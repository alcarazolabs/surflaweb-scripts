# -*- coding: utf-8 -*-
"""
Created on Sun Oct 24 11:16:52 2021

@author: Freddy Alcarazo -  @surflawet

"""

import os
from os import path
import shutil

Source_Path = 'C:\\python\\fredd\\Imagenes\\train'
Destination = 'C:\\python\\fredd\\Imagenes\\train_renamed'
#dst_folder = os.mkdir(Destination)


def main():
    for count, filename in enumerate(os.listdir(Source_Path)):
        count = count+1
        dst =  "lemon_" + str(count) + ".jpg"

        # rename all the files
        os.rename(os.path.join(Source_Path, filename),  os.path.join(Destination, dst))


# Driver Code
if __name__ == '__main__':
    main()