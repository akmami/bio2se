#!/bin/bash


CWD_PATH=$(pwd)

PG_USER="postgres";
DB_NAME="ccdb";
DUMP_BCB="bcb";
CSV_OUTPUT="$CWD_PATH/bcb.csv";


# Check if dump file exists
if [ ! -e "$DUMP_BCB" ]; then
    echo "$DUMP_BCB file doesn't exist.";
    exit 1;
fi


# Create output file
if [ ! -e "$CSV_OUTPUT" ]; then
    touch "$CSV_OUTPUT";
fi

sudo chmod +x "$CSV_OUTPUT";


# define commands and queries
case "$(uname -s)" in
  Linux*|Darwin*)
    CMD_DB_CHECK="sudo -u $PG_USER psql -lqt -U $PG_USER | cut -d \| -f 1 | grep -qw $DB_NAME";
    CMD_CREATE_DB="sudo createdb -U $PG_USER -W $DB_NAME";
    CMD_RESTORE="psql -U $PG_USER -d $DB_NAME -W -f $DUMP_BCB";
    CMD_EXTRACT="sudo psql -U $PG_USER -d $DB_NAME -W -c";
    ;;

  CYGWIN*|MINGW32*|MSYS*)
    # Windows
    CMD_DB_CHECK="sudo psql.exe -U $PG_USER -lqt | cut -d \| -f 1 | grep -qw $DB_NAME";
    CMD_CREATE_DB="sudo createdb.exe -U $PG_USER -W $DB_NAME";
    CMD_RESTORE="psql.exe -U $PG_USER -d $DB_NAME -W -f $DUMP_BCB";
    CMD_EXTRACT="sudo psql.exe -U $PG_USER -d $DB_NAME -W -c";
    ;;

  *)
    echo "Unsupported operating system";
    exit 1;
    ;;
esac


# Create the database
echo "Creating database $DB_NAME.";

if ! eval "$CMD_DB_CHECK"; then
  eval "$CMD_CREATE_DB";
  echo "Database $DB_NAME created.";
else;
  echo "Database $DB_NAME already exists.";
fi


# Restore the database from the dump

eval "$CMD_RESTORE";


# Create .csv file for the given format
# function_id_one, function_id_two, syntactic_type, internal, text_of_funciton_id_one, text_of_funciton_id_two
# output file is 25.7 GB
eval "$CMD_EXTRACT \"
COPY (
  SELECT
    function_id_one,
    function_id_two,
    syntactic_type,
    internal,
    (SELECT text FROM public.pretty_printed_functions WHERE function_id = clones.function_id_one) AS pretty_function_one,
    (SELECT text FROM public.pretty_printed_functions WHERE function_id = clones.function_id_two) AS pretty_function_two
  FROM public.clones
) TO './bcb.csv' WITH CSV HEADER;\""