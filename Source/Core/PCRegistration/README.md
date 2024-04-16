# PCRegistration: Point-Cloud Registration

This folder contains code that is used to carry out point-cloud registration,
such as is needed to align and map a proposed emulation network to a
ground-truth network where the cell bodies found are approximately in the
right relative positions, but with unpredictable rotation or translation of
the whole network.

## Simple Registration

This process does the following:

1. Requests from NES the soma positions of the ground-truth network.
2. Requests from NES the soma positions of the emulation network.
3. Centers each network.
4. Runs through a series of angles of rotation over all axes and
   calculates a proximity score between nearest cell bodies in
   the two networks. The total score for a specific combination of
   angles is used to determine the best combination of rotations.
5. Returns a list of corresponding indices based on the best
   registration. That list of indices can be used to compare
   neurons in one network with neurons in the other when carrying
   out validation processes.

There are innumerable registration algorithms. There is almost certainly a
much more efficient way to do this, especially, when the number of neurons
becomes large. At that point, the registration process becomes very similar
to registration of point clouds or meshes that is used with images.

