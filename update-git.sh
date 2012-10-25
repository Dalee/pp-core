#!/bin/bash
# */5 * * * * /home/mantonov/gitlibpp/update-git.sh

#Automatic transport commits from svn to git

# cd to repo
cd $( dirname ${BASH_SOURCE[0]} );
git pull origin master
git checkout master
git svn fetch
git merge remotes/git-svn
git push origin master

