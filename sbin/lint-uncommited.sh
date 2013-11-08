#!/bin/bash
git status -s | cut -f 3 -d ' ' | xargs -L 1 -I % php54 -l %
