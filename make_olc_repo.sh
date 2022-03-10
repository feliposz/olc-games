# Kept this as a reference to the operations I did
# to merge all the git repositories into one
# while keeping separate folders and commit history

# Rewrite commit message history adding a prefix to messages
# Also, move file paths as if they were always part of a subdirectory

# Inspired by: https://stackoverflow.com/a/21495718

for PREFIX in *
do
  if [ -d "$PREFIX" ]
  then
    pushd $PREFIX
    git filter-branch \
      --msg-filter 'sed -e "s,^,&'$PREFIX': ,"' \
      --index-filter '
        git ls-files -s |
        sed "s,\t,&'"$PREFIX"'/," |
        GIT_INDEX_FILE=$GIT_INDEX_FILE.new git update-index --index-info &&
        mv $GIT_INDEX_FILE.new $GIT_INDEX_FILE
      ' HEAD
    mv *.vcxproj.user $PREFIX
    ls -l
    git log --oneline
    popd
  fi
done

# Create base repository to group them all

mkdir olc-games
cd olc-games
git init
touch README.md
git add README.md
git commit -m 'One-Lone-Coder Games: Initial commit'

# Add original repositories as subdirectories of the new one

git pull --allow-unrelated-histories ../MatrixRain
git pull --allow-unrelated-histories ../ConsoleWebCam
git pull --allow-unrelated-histories ../CaveDiver
git pull --allow-unrelated-histories ../Snake
git pull --allow-unrelated-histories ../Tetris
git pull --allow-unrelated-histories ../SoundSynth
git pull --allow-unrelated-histories ../ArcadeRacing
git pull --allow-unrelated-histories ../AStar
git pull --allow-unrelated-histories ../Asteroids
git pull --allow-unrelated-histories ../Conway
git pull --allow-unrelated-histories ../PerlinNoise
git pull --allow-unrelated-histories ../FPS
git pull --allow-unrelated-histories ../Frogger
git pull --allow-unrelated-histories ../MazeGen
git pull --allow-unrelated-histories ../Platformer
git pull --allow-unrelated-histories ../RTS
git pull --allow-unrelated-histories ../Splines
git pull --allow-unrelated-histories ../Worms
git pull --allow-unrelated-histories ../Engine3D
git pull --allow-unrelated-histories ../CarCrime
git pull --allow-unrelated-histories ../RPG
