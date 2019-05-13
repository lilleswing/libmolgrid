import pytest
import molgrid
import numpy as np
try:
    import pybel #2.0
except ImportError:
    from openbabel import pybel  #3.0
from pytest import approx

#manually construct an example and test merge_coordinates
def test_example_merge():
    m = pybel.readstring('smi','c1ccccc1CO')
    m.addh()
    m.make3D()
    
    c = molgrid.CoordinateSet(m,molgrid.ElementIndexTyper())
    c2 = molgrid.CoordinateSet(m)

    c2.make_vector_types() #this should not screw up index types
    
    ex = molgrid.Example()
    ex.coord_sets.append(c)
    ex.coord_sets.append(c2)
    assert ex.type_size() == (c.max_type + c2.max_type)
    assert ex.coordinate_size() == (c.coord.dimension(0)+c2.type_index.size())
    
    c3 = ex.merge_coordinates()
    assert c3.coord.tonumpy().shape == (24,3)
    
    t = np.concatenate([c.type_index.tonumpy(),c2.type_index.tonumpy()+c.max_type])
    assert np.array_equal(t, c3.type_index.tonumpy())
    
    #test merging without unique types, which makes no sense
    c4 = ex.merge_coordinates(0,False)
    assert c4.coord.tonumpy().shape == (24,3)
    t = np.concatenate([c.type_index.tonumpy(),c2.type_index.tonumpy()])
    assert np.array_equal(t, c4.type_index.tonumpy())
    
    #test sliced merging
    c5 = ex.merge_coordinates(1,False)
    assert c5.coord.tonumpy().shape == (8,3) #no hydrogens in this slice