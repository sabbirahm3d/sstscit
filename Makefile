.PHONY: help
help:
	@echo "make help"
	@echo "      this message"
	@echo "==== Targets outside container ===="
	@echo "make docker"


docker: docker_build docker_run
docker_build:
	time docker build -f Dockerfile -t sst_10 .
#	time DOCKER_BUILDKIT=1 docker build -f Dockerfile -t sst_10 .
# https://brianchristner.io/what-is-docker-buildkit/
docker_run:
	docker run -it --rm -v `pwd`:/scratch sst_10 /bin/bash 