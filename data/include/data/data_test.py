import skimage
from skimage import transform
from skimage import io
import numpy as np

import glob


print("Loading Omniglot data...")
imagedata = []
imagefilenames=[]
for basedir in ('./images_background/','./images_evaluation/'):
    alphabetdirs = glob.glob(basedir+'*')
    print(alphabetdirs[:4])
    for alphabetdir in alphabetdirs:
        print(alphabetdir+"/*")
        chardirs = glob.glob(alphabetdir+"/*")
        for chardir in chardirs:
            chardata = []
            charfiles = glob.glob(chardir+'/*')
            for fn in charfiles:
                filedata = skimage.io.imread(fn) / 255.0 #plt.imread(fn)
                # print(fn)
                chardata.append(filedata)
                imagedata.append(chardata)
                imagefilenames.append(fn)
                # imagedata is now a list of lists of numpy arrays
                # imagedata[CharactertNumber][FileNumber] -> numpy(105,105)
    np.random.shuffle(imagedata)  # Randomize order of characters
    print(len(imagedata))
    print(imagedata[1][2].shape)
    print("Data loaded!")
