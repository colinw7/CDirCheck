#include <CDirCheck.h>
#include <CArgs.h>

int
main(int argc, char **argv)
{
  CArgs args("-Z:f (list zero sized files) "
             "-N:f (list bad file names) "
             "-D:f (list duplicate files) "
             "-L:f (list bad links) "
             "--zero:f (list zero sized files) "
             "--bad_names:f (list bad file names) "
             "--duplicates:f (list duplicate files) "
             "--bad_links:f (list bad links) "
             "--dup_names:f (list duplicate file names) "
             "--all:f (enabled all checks) "
             "--bigger:i (list files bigger than specified size) "
             "--smaller:i (list files smaller than specified size) "
             "--match_dir:s (only include directories matching pattern) "
             "--match_file:s (only include files matching pattern) "
             "--ignore_dir:s (ignore directories matching pattern) "
             "--ignore_file:s (ignore files matching pattern) ");

  args.parse(&argc, argv);

  if (args.isHelp()) exit(1);

  std::vector<std::string> files;

  for (int i = 1; i < argc; ++i)
    files.push_back(argv[i]);

  bool zero_length = args.getBooleanArg("-Z") || args.getBooleanArg("--zero");
  bool bad_names   = args.getBooleanArg("-N") || args.getBooleanArg("--bad_names");
  bool duplicates  = args.getBooleanArg("-D") || args.getBooleanArg("--duplicates");
  bool bad_links   = args.getBooleanArg("-L") || args.getBooleanArg("--bad_links");
  bool dup_names   = args.getBooleanArg("--dup_names");
  bool all         = args.getBooleanArg("--all");
  bool bigger_set  = args.isIntegerArgSet("--bigger");
  int  bigger      = args.getIntegerArg  ("--bigger");
  bool smaller_set = args.isIntegerArgSet("--smaller");
  int  smaller     = args.getIntegerArg  ("--smaller");

  std::string matchDir   = args.getStringArg("--match_dir");
  std::string matchFile  = args.getStringArg("--match_file");
  std::string ignoreDir  = args.getStringArg("--ignore_dir");
  std::string ignoreFile = args.getStringArg("--ignore_file");

  if (files.empty())
    files.push_back(".");

  uint numFiles = files.size();

  for (uint i = 0; i < numFiles; ++i) {
    CDirCheck check(files[i]);

    if (matchDir   != "") check.setMatchDir  (matchDir  );
    if (matchFile  != "") check.setMatchFile (matchFile );
    if (ignoreDir  != "") check.setIgnoreDir (ignoreDir );
    if (ignoreFile != "") check.setIgnoreFile(ignoreFile);

    if (zero_length || all) check.addTest(CDirCheck::ZERO_LENGTH);
    if (bad_names   || all) check.addTest(CDirCheck::BAD_NAME   );
    if (duplicates  || all) check.addTest(CDirCheck::DUPLICATE  );
    if (bad_links   || all) check.addTest(CDirCheck::BAD_LINK   );
    if (dup_names   || all) check.addTest(CDirCheck::DUP_NAME   );

    if (bigger_set ) { check.addTest(CDirCheck::BIGGER ); check.setBigger (bigger); }
    if (smaller_set) { check.addTest(CDirCheck::SMALLER); check.setSmaller(smaller); }

    check.exec();
  }

  return 0;
}
