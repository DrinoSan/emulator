#include <chrono>
#include <fstream>
#include <random>

#include "Chip8.h"

Chip8::Chip8()
    : randGen{ std::chrono::system_clock::now().time_since_epoch().count() }
{
   pc = START_ADDRESS;

   // Initialize RNG
   randByte = std::uniform_int_distribution<uint8_t>( 0, 255U );

   // Load fonts into memory
   for ( unsigned int i = 0; i < FONTSET_SIZE; ++i )
   {
      memory[ FONTSET_START_ADDRESS + i ] = fontset[ i ];
   }
}

void Chip8::LoadROM( char const* filename )
{
   // Open the file as a stream of binary and move the file pointer to the end
   std::ifstream file( filename, std::ios::binary | std::ios::ate );

   if ( file.is_open() )
   {
      // Get size of file and allocate a buffer to hold the contents
      std::streampos size   = file.tellg();
      char*          buffer = new char[ size ];

      // Go back to the beginning of the file and fill the buffer
      file.seekg( 0, std::ios::beg );
      file.read( buffer, size );
      file.close();

      // Load the ROM contents into the Chip8's memory, starting at 0x200
      for ( long i = 0; i < size; ++i )
      {
         memory[ START_ADDRESS + i ] = buffer[ i ];
      }

      // Free the buffer
      delete[] buffer;
   }
}

// OpCode Operations
void Chip8::OP_00E0()
{
   memset( video, 0, sizeof( video ) );
}

void Chip8::OP_00EE()
{
   --sp;
   pc = stack[ sp ];
}

void Chip8::OP_1NNN()
{
   uint16_t address = opcode & 0x0FFFu;

   pc = address;
}

void Chip8::OP_2NNN()
{
   stack[ sp ] = pc;
   ++sp;

   uint16_t address = opcode & 0x0FFFu;
   pc               = address;
}

// The interpreter compares register Vx to kk, and if they are equal, increments
// the program counter by 2.
void Chip8::OP_3XKK()
{
   uint8_t Vx   = ( opcode & 0x0F00u ) >> 8u;
   uint8_t byte = opcode & 0x00FFu;

   if ( registers[ Vx ] == byte )
   {
      pc += 2;
   }
}

// The interpreter compares register Vx to kk, and if they are not equal,
// increments the program counter by 2.
void Chip8::OP_4XKK()
{
   uint8_t Vx   = ( opcode & 0x0F00u ) >> 8u;
   uint8_t byte = opcode & 0x00FFu;

   if ( registers[ Vx ] != byte )
   {
      pc += 2;
   }
}

// The interpreter compares register Vx to register Vy, and if they are equal,
// increments the program counter by 2.
void Chip8::OP_5XY0()
{
   uint8_t Vx = ( opcode & 0x0F00u ) >> 8u;
   uint8_t Vy = ( opcode & 0x00F0u ) >> 4u;

   if ( registers[ Vx ] == registers[ Vy ] )
   {
      pc += 2;
   }
}

// The interpreter puts the value kk into register Vx
void Chip8::OP_6XKK()
{
   uint8_t Vx   = ( opcode & 0x0F00u ) >> 8u;
   uint8_t byte = ( opcode & 0x00FFu );

   registers[ Vx ] = byte;
}

// Adds the value kk to the value of register Vx, then stores the result in Vx
void Chip8::OP_7XKK()
{
   uint8_t Vx   = ( opcode & 0x0F00u ) >> 8u;
   uint8_t byte = ( opcode & 0x00FFu );

   registers[ Vx ] += byte;
}

// Stores the value of register Vy in register Vx.
void Chip8::OP_8XY0()
{
   uint8_t Vx = ( opcode & 0x0F00u ) >> 8u;
   uint8_t Vy = ( opcode & 0x00F0u ) >> 4u;

   registers[ Vx ] = registers[ Vy ];
}

// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
// A bitwise OR compares the corrseponding bits from two values, and if either bit is 1,
// then the same bit in the result is also 1. Otherwise, it is 0.
void Chip8::OP_8XY1()
{
   uint8_t Vx = ( opcode & 0x0F00u ) >> 8u;
   uint8_t Vy = ( opcode & 0x00F0u ) >> 4u;

   Vx |= Vy;
}

// Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
// A bitwise AND compares the corrseponding bits from two values, and if both bits are 1,
// then the same bit in the result is also 1. Otherwise, it is 0.
void Chip8::OP_8XY2()
{
   uint8_t Vx = ( opcode & 0x0F00u ) >> 8u;
   uint8_t Vy = ( opcode & 0x00F0u ) >> 4u;

   registers[ Vx ] &= registers[ Vy ];
}

// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
// An exclusive OR compares the corrseponding bits from two values, and if the bits are not both the same,
// then the corresponding bit in the result is set to 1. Otherwise, it is 0.
void Chip8::OP_8XY3()
{
   uint8_t Vx = ( opcode & 0x0F00u ) >> 8u;
   uint8_t Vy = ( opcode & 0x00F0u ) >> 4u;

   registers[ Vx ] ^= registers[ Vy ];
}
