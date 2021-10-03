# !/usr/bin/env python

import math

from vtkmodules.vtkCommonCore import vtkMath


def main():
    p0 = (0, 0, 0)
    p1 = (1, 1, 1)

    distSquared = vtkMath.Distance2BetweenPoints(p0, p1)

    dist = math.sqrt(distSquared)

    print('p0 = ', p0)
    print('p1 = ', p1)
    print('distance squared = ', distSquared)
    print('distance = ', dist)


if __name__ == '__main__':
    main()
