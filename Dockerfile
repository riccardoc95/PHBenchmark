FROM mambaorg/micromamba:1.5.10

USER root
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        bash \
        build-essential \
        ca-certificates \
        cmake \
        git \
        time \
    && rm -rf /var/lib/apt/lists/*

USER $MAMBA_USER
WORKDIR /app

COPY --chown=$MAMBA_USER:$MAMBA_USER environment.yml pyproject.toml README.md ./
COPY --chown=$MAMBA_USER:$MAMBA_USER PixHomology ./PixHomology
COPY --chown=$MAMBA_USER:$MAMBA_USER phbenchmark ./phbenchmark

RUN micromamba env create -y -f environment.yml \
    && micromamba clean --all --yes

ENV PATH=/opt/conda/envs/phbenchmark/bin:$PATH
ENV CONDA_DEFAULT_ENV=phbenchmark
ENV PYTHONUNBUFFERED=1

ARG DATASET=all
RUN phbenchmark download --datasets_dir /app/datasets --dataset "$DATASET"

COPY --chown=$MAMBA_USER:$MAMBA_USER docker-entrypoint.sh /usr/local/bin/phbenchmark-docker

WORKDIR /work
ENTRYPOINT ["/usr/local/bin/phbenchmark-docker"]
