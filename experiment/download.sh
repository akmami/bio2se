#!/bin/bash


DATASET_LINK="https://rmy0rw.bn.files.1drv.com/y4mv4s8vvt7K7WH0HgsHbbwU-5ern2laoOVJZpKioBbF8Zdz8pLBsQ7QhOeKtHriJo4Y70sASti5OgllmqEGqHE_r0mgpbArOzkigTQCrSAy14eAY-eCMBRWvv80DEX8B2FzB1VdaFjHilBQKGEEum1QyibQttrgRQmkheqijyJdGzIz3F3OfRoOO3G2tqXmx7PrqYYzVRcdbAjPid8H9W1QQ"
BCB_FILE=bcb.pgdump.tar.gz

if [ ! -e "$BCB_FILE" ]; then
    wget $DATASET_LINK -O "$BCB_FILE"
fi


tar -xvf "$BCB_FILE"
