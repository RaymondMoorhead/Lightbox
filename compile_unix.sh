#!/bin/bash

g++ -o Bin/GraphicsDemo.out -IDependencies Source/*.cpp Source/Graphics/*.cpp Source/Graphics/LowLevel/*.cpp Dependencies/glad/glad.c Dependencies/imgui/*.cpp -ldl -lglfw

if [ $? -eq 0 ]; then
  echo "Your program has successfully compiled, here is the output:"
  ./run.sh
else
  echo "The program failed to compile"
  exit 1
fi