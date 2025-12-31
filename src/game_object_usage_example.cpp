#include <iostream>
#include "game_object.hpp"


struct HealthTag{};
struct PosXTag{};
struct PosYTag{};

using Health = dod::Field<HealthTag, int>;
using PosX   = dod::Field<PosXTag, float>;
using PosY   = dod::Field<PosYTag, float>;

//using Creature   = ObjectType<Health, PosX, PosY>;
//using Projectile = ObjectType<PosX, PosY>;
using Creature2 = dod::ObjectType<Health, PosX, PosY>;


static_assert(GAME_OBJECT_V2_HPP_VERSION == 12345);

template<class Obj>
void print_creature(const char* title, Obj& c) {
  // Bind by reference (no copies)
  auto h = c.template column<Health>();
  auto x = c.template column<PosX>();
  auto y = c.template column<PosY>();

  std::cout << title << "\n";
  std::cout << "size = " << c.size() << "\n";
  //std::cout << std::fixed //<< std::setprecision(2);

  for (std::size_t i = 0; i < c.size(); ++i) {
    std::cout << "  i=" << i
              << "  Health=" << h[i]
              << "  PosX="   << x[i]
              << "  PosY="   << y[i]
              << "\n";
  }
  std::cout << "\n";
}




int main() {
  Creature2 c;
  static_assert(c.sanity_marker == 123);
  //c.column<Health>().push_back(100);
  c.reserve(1024);
  c.emplace(100, 10.0f, 20.0f);   // Health, PosX, PosY (in template order)
  c.emplace( 50, -1.5f,  3.0f);
  c.emplace(999,  0.0f,  0.0f);

  print_creature("Before update:", c);


  dod::TestType test;
  auto test_size = test.size();

   // Mutate in-place
  auto hx = c.column<Health>();
  auto x  = c.column<PosX>();
  auto y  = c.column<PosY>();

  for (std::size_t i = 0; i < c.size(); ++i) {
    hx[i] -= 1;
    x[i]  += 0.1f;
    y[i]  += 0.2f;
  }


  print_creature("After update:", c);


  // Basic consistency check

  // Basic consistency check
  if (c.column<Health>().size() == c.column<PosX>().size() &&
      c.column<PosX>().size() == c.column<PosY>().size()) {
    std::cout << "Column sizes are consistent.\n";
  } else {
    std::cout << "ERROR: Column sizes mismatch!\n";
  }

}
