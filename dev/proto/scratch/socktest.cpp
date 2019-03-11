/**
Simple 4-bit Up-Counter Model with one clock
*/

#include "socktest.hpp"
#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>

// Component Constructor
socktest::socktest(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id), m_sh_in(socket(AF_UNIX, SOCK_STREAM, 0)),
      m_clock(params.find<std::string>("clock", "")),
      m_proc(params.find<std::string>("proc", "")),
      m_ipc_port(params.find<std::string>("ipc_port", "")) {

    // Initialize output
    m_output.init("\033[32mgalois_lfsr-" + getName() + "\033[0m (pid: " +
                  std::to_string(getpid()) + ") -> ", 1, 0, SST::Output::STDOUT);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<socktest>(this, &socktest::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

socktest::~socktest() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying socktest...\n");

}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void socktest::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    std::cout << "Master pid: " << getpid() << std::endl;

    if (!fork()) {

        char *args[] = {&m_proc[0u], &m_ipc_port[0u], nullptr};
        m_output.verbose(CALL_INFO, 1, 0,
                         "Forking process %s (pid: %d) as \"%s\"...\n", args[0], getpid(),
                         m_proc.c_str());
        execvp(args[0], args);

    } else {

        m_sh_in.set_params(m_ipc_port);
        m_sh_in.recv();
        std::cout << "[pid]=" << m_sh_in.get<int>("pid") << std::endl;

        m_output.verbose(CALL_INFO, 1, 0, "Launched black box and connected to socket\n");

    }

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void socktest::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");

}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool socktest::tick(SST::Cycle_t current_cycle) {

    std::cout << "<----------------------------------------------------" << std::endl;

    bool keep_send = current_cycle < 39, keep_recv = current_cycle < 38;

    m_sh_in.set("clock", current_cycle, SC_UINT_T);
    m_sh_in.set_state(true);
    m_sh_in.set("reset", 1);

    // turn module off at 52 ns
    if (current_cycle >= 3) {
        if (current_cycle == 3) {
            std::cout << "RESET OFF" << std::endl;
        }
        m_sh_in.set("reset", 0);
    }

    // turn module off at 52 ns
    if (current_cycle >= 38) {
        if (current_cycle == 38) {
            std::cout << "GALOIS LFSR MODULE OFF" << std::endl;
        }
        m_sh_in.set_state(false);
    }

    if (keep_send) {

        m_sh_in.send();
    }

    if (keep_recv) {

        m_sh_in.recv();
        m_output.verbose(CALL_INFO, 1, 0, "%d\n", m_sh_in.get<int>("data_out"));

    }

    std::cout << "---------------------------------------------------->" << std::endl;

    return false;

}
