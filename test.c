// #include <stdio.h>
// #include <stdint.h>
// /* Figure out randomness
// Address a static point in memory
//  */
// struct Coordinate {
//     uint8_t x;
//     uint8_t y;
// };

// struct Block {
//     uint8_t shape;
//     uint8_t rot;
//     struct Coordinate origin;
// };

// inline uint32_t random_u32(uint32_t prev) {
//     return prev*48271U;
// }

// struct Block SpawnBlock() {
//     struct Block b;
//     uint32_t seed = 134775813U;
//     b.shape = random_u32(seed);
//     b.rot = 0;
//     b.origin.x = 0;
//     b.origin.y = 0;
//     return b;
// }

// int main(void) {
//     struct Block cb = SpawnBlock();
//     printf("%d\n", cb.shape);
//     return 0;
// }