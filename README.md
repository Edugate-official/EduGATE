#EduGATE

**Disclaimer: Early stage and work in progress.**

EduGATE introduces students to the fundamental physical aspects of medical imaging devices. It is based on the well-established [opensource software](https://github.com/OpenGATE) GATE and enables scientists to perform numerical simulations in medical imaging and radiotherapy [1, 2, 3]. It is devolped and maintained by the [Opengate Collaboration](http://www.opengatecollaboration.org/).

The EduGATE modules (and the related [publication of Pietrzyk et al. 2013](https://pubmed.ncbi.nlm.nih.gov/22909417/)) can be found on the following [website](https://gate.uca.fr/download/edugate#/admin), but until now an official software repository is missing.

The experienced user can already make use of the available EduGATE modules. This repository is dedicated to making the modules ~~more accessible to inexperienced users~~ nearly instant and reproducible installable and usable for everyone.

1. [GATE: a simulation toolkit for PET and SPECT](https://pubmed.ncbi.nlm.nih.gov/15552416/)
2. [GATE V6: a major enhancement of the GATE simulation platform enabling modelling of CT and radiotherapy](https://pubmed.ncbi.nlm.nih.gov/21248393/)
3. [A review of the use and potential of the GATE Monte Carlo simulation code for radiation therapy and dosimetry applications](https://pubmed.ncbi.nlm.nih.gov/24877844/)

## Version

First Docker based EduGATE deployment

- EduGATE 2018 modules
- GATE release tag 8.2

## Prerequisites

- A working [ROOT 6](https://root.cern/) installation
- A working [Docker](https://www.docker.com/) Environment

## Ok, I have a clean install of one of the major OS, how can I start?

### Ubuntu

- Install root via conda
- Install docker
- Install docker-compose

### Mac or Windows

- Install root via conda
- Install Docker Desktop

## Docker and ROOT 6 are working, now I would like to start the simulations...

In the moment just a few modules are usable. To call them type:

1. `sh start_tof.sh`
1. `sh start_cc.sh`
1. `sh start_gamma.sh` (buggy)

in your terminal and follow the GUI.

## Prospects

- Update to EduGATE 2020
- Test with the GATE 9
- GUI and analysis of the EduGATE modules via a Jupyter Notebook Interface to the Docker Container (so that just Docker and Chrome are needed)
- Getting the visualization of the GATE simulation out of the Docker container
- Full Description of the EduGATE modules in [this repository](https://github.com/Edugate-official/EduGATE-Docs)
