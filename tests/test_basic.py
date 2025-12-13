import flash_embed


def test_add():
    """Test the 'Hello World' add function from C++ extension."""
    assert flash_embed.add(1, 2) == 3
    assert flash_embed.add(-1, 1) == 0
    print("\n✅ C++ Extension 'add' function verification passed!")
