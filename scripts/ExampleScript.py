# imports
import numpy as np
import glob

# load file
files = glob.glob('../*.npz')
dict = np.load(files[0])

figure(1)
clf()
# get array of all the energy deposits
# need to throw our phe hits, where no energy is depsited
energy_deposits = dict['energy_deposition'][np.where(dict['energy_deposition'] > 0)]
hist(energy_deposits, bins=20, range=[0, 20])
xlabel('Recoil Energy (keV)')

figure(2)
clf()
# get R^2 and Z
# again, we need to avoid the entries where no energy is deposited
inds = np.where(dict['energy_deposition'] > 0)
X = dict['position'][inds, 0]
Y = dict['position'][inds, 1]
Z = dict['position'][inds, 2]

# turn X and Y into R^2
R2 = np.array([X[i]*X[i] + Y[i]*Y[i] for i in range(len(X))])
plot(R2, Z, 'ko')
xlim([0, 625])
ylim([0, 55])
xlabel('R^2 (cm^2)')
ylabel('Z (cm)')

