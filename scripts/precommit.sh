## Git precommit-hook script - tuna by sme mali dat vsetky testy tak,
## aby git nedovolil commit ak neprejdu

#!/bin/bash

mkdir -p .buildir && cp .buildir
git -C ../ archive --format tar 
