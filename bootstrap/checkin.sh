#!/usr/bin/env bash

comment="$@"

if [[ -z "$comment" ]]
then
  echo "Expected comment for commit. $@ is used to compose comment out all of arguments."
  exit 1
fi

git add .
git commit -m "$comment"
git push
