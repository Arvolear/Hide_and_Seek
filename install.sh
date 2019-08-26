#!/bin/bash

mv libs /usr/local/lib/HideAndSeek

if [[ $LD_LIBRARY_PATH != *"HideAndSeek"* ]]
then
    echo -e "\nexport LD_LIBRARY_PATH=\"\$LD_LIBRARY_PATH:/usr/local/lib/HideAndSeek\"" >> ~/.bashrc
fi

source /home/$USER/.bashrc
