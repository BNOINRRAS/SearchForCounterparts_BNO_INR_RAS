import sys

from astropy.table import Table
from astropy import units as u
import astropy_healpix as ah
import numpy as np
from mocpy import MOC

import matplotlib.pyplot as plt
from matplotlib.pyplot import figure

# Обработка fits-файла
skymap = Table.read(sys.argv[1])

level, ipix = ah.uniq_to_level_ipix(skymap['UNIQ'])
nside = ah.level_to_nside(level)
pixelArea = ah.nside_to_pixel_area(nside).to_value(u.steradian)
probDensity = skymap['PROBDENSITY']
probabilities = pixelArea * probDensity

# Construct credible level map
i = np.flipud(np.argsort(probDensity))
cls = np.empty(len(skymap))
cls[i] = np.cumsum(probabilities[i])

# Get boundaries of 90% credible level
credibleLevel = 0.9
keep = (cls <= credibleLevel)
moc = MOC.from_healpix_cells(ipix[keep], level[keep], max(level) )
boundaries = moc.get_boundaries()

eventName = sys.argv[2]
eventName = eventName.replace('/','.')
# Запись файла с координатами контура
writeFile = open('GW_Localization_Contours_and_skymaps/' + eventName + '.txt', 'w')
writeFile.write('Contours ' + repr(len(boundaries)) + '\n' )
for i in range(len(boundaries)):
	writeFile.write('Lines ' + repr(len(boundaries[i])) + '\n' )
	for j in range(len(boundaries[i])):
		writeFile.write(repr( round(boundaries[i][j].ra.deg, 2) ) + '\t' + repr( round(boundaries[i][j].dec.deg, 2) ) + '\n' )
writeFile.close()

# Создание png-изображения
fig = plt.figure()
ax = fig.add_subplot(111, projection='mollweide')

X = np.array([])
Y = np.array([])

for i in range(len(boundaries)):
	X = np.append(X, ( 2.0 * np.pi - boundaries[i].ra.rad ) - np.pi )
	Y = np.append(Y, boundaries[i].dec.rad)

ax.set_xticks([-180.0 * ( np.pi / 180.0 ), -165.0 * ( np.pi / 180.0 ), -150.0 * ( np.pi / 180.0 ),
               -135.0 * ( np.pi / 180.0 ), -120.0 * ( np.pi / 180.0 ), -105.0 * ( np.pi / 180.0 ),
                -90.0 * ( np.pi / 180.0 ), -75.0 * ( np.pi / 180.0 ), -60.0 * ( np.pi / 180.0 ),
                -45.0 * ( np.pi / 180.0 ), -30.0 * ( np.pi / 180.0 ), -15.0 * ( np.pi / 180.0 ),
                  0.0 * ( np.pi / 180.0 ), 15.0 * ( np.pi / 180.0 ), 30.0 * ( np.pi / 180.0 ),
                 45.0 * ( np.pi / 180.0 ), 60.0 * ( np.pi / 180.0 ), 75.0 * ( np.pi / 180.0 ),
                 90.0 * ( np.pi / 180.0 ), 105.0 * ( np.pi / 180.0 ), 120.0 * ( np.pi / 180.0 ),
                135.0 * ( np.pi / 180.0 ), 150.0 * ( np.pi / 180.0 ), 165.0 * ( np.pi / 180.0 ),
                180.0 * ( np.pi / 180.0 )])
ax.set_xticklabels(['0h', '', '','21h', '', '','18h', '', '','15h', '', '','12h', '', '','9h', '', '',
					'6h', '', '','3h', '', '','0h'])

ax.grid(True)
ax.grid(linestyle='dashed')
ax.set_axisbelow(True)
ax.scatter(X, Y, s = 1.0, c = 'b', marker = 'o', alpha = 0.2)

fig.set_size_inches(4.0, 2.0)
#fig.set_dpi(100)

#plt.show()
plt.savefig('GW_Localization_Contours_and_skymaps/' + eventName + '.png')
