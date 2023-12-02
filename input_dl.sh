#/bin/bash

ROOT_DIR=$(dirname $0)
ENV_PATH=$ROOT_DIR/.env
SESSION_TOKEN=$(grep SESSION_TOKEN "$ENV_PATH" | sed "s/SESSION_TOKEN=//")

DAY=$1
YEAR=$2

[ -z "$DAY" ] && DAY=1
[ -z "$YEAR" ] && YEAR=2023

INPUT="https://adventofcode.com/$YEAR/day/$DAY/input"
OUTPUT="$ROOT_DIR/$YEAR/input_$(printf %02d $DAY).txt"

mkdir -p "$ROOT_DIR/$YEAR"
curl -b session=$SESSION_TOKEN $INPUT --output $OUTPUT
