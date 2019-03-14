#include "../modules/fib_lfsr.hpp"

#include "../../src/sstscit.hpp"

#include <sys/un.h>

int sc_main(int argc, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<sc_uint<4> > data_out;

    // Connect the DUT
    fib_lfsr DUT("FIB_LFSR");
    DUT.clock(clock);
    DUT.reset(reset);
    DUT.data_out(data_out);
    // ---------- SYSTEMC UUT INIT ---------- //

    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
    SignalSocket sh_in(socket(AF_UNIX, SOCK_STREAM, 0), false);
    sh_in.set_addr(argv[1]);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    sh_in.set("pid", getpid(), SC_UINT_T);
    sh_in.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    while (true) {

        sc_start(1, SC_NS);

        // RECEIVING
        sh_in.recv();

        if (!sh_in.alive()) {
            break;
        }
        clock = sh_in.get_clock_pulse("clock");
        reset = sh_in.get<bool>("reset");
        std::cout << "\033[33mFIB LFSR\033[0m -> clock: "
                  << sc_time_stamp() << " | reset: " << sh_in.get<bool>("reset")
                  << " -> fib_lfsr_out: " << data_out << std::endl;

        // SENDING
        sh_in.set("data_out", data_out, SC_UINT_T);
        sh_in.send();

    }

    return 0;

}
