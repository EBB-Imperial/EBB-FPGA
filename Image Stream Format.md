# Image Stream Format

Each frame consists of the following:

- Sync word: ASCII `UUUUUUUUUUUUUUUw`, i.e. `([0x55] * 15) + [0x77]`

- 235 lines of image data each consisting of 320 pixels

    - Each pixel is RGB 565, stored in two bytes, sent in **little-endian** order — so each line is 640 bytes. As a 16-bit word the format is:
        ```
        Component : rrrrr gggggg bbbbb
         Bit Pos  : 15 11 10   5 4   0
        ```

        Or if split at the byte boundary,
        ```
        Component : rrrrrggg gggbbbbb
         Bit Pos  : 15     8 7      0
        ```
        
        Due to Nios II being little-endian, these two bytes are sent in reverse order:
        ```
        gggbbbbb rrrrrggg gggbbbbb rrrrrggg gggbbbbb rrrrrggg ...
        \_______________/ \_______________/ \_______________/
             pixel 1           pixel 2           pixel 3
        ```

RGB 565 is converted to RGB 888 by scaling each component to fit the range 0~255:

```
 rrrrr << 3 -> rrrrr000
gggggg << 2 -> gggggg00
 bbbbb << 3 -> bbbbb000
```

