#include <stdio.h>
#include <stdlib.h>
#include <argtable2.h>
#include <ios>
#include <string>
#include <iostream>
#include <iomanip>

#include "floatfann.h"
#include "fann_cpp.h"

void usage(void **argtable, const char *progname) {
  FILE *fp = stdout;
  fprintf(fp, "Usage: %s ", progname);
  arg_print_syntaxv(fp, argtable, "\n");
  arg_print_glossary(fp, argtable, " %-50s %s\n");
}

int main(int argc, char**argv) {
  const char *progname = "train";
  struct arg_file *At = arg_file1(NULL, NULL, "<trainfile>", "input file (training)");
  struct arg_file *Amodel = arg_file1("s", "save", "<model>", "output model file");
  struct arg_int *Aiters = arg_int0("i", "maxiter", "<n>", "maximum number of iterations (default 100)");
  Aiters->ival[0] = 100;
  struct arg_int *Ahidden = arg_int0("h", "hidden", "<n>", "number of hidden nodes (default 50)");
  Ahidden->ival[0] = 50;
  struct arg_dbl *Alr = arg_dbl0("l", "learningrate", "<d>", "initial learning rate (default 0.1)");
  Alr->dval[0] = 0.1;
  struct arg_lit *Ahelp = arg_lit0("h", "help", "show help and exit");
  struct arg_end *Aend = arg_end(20);

  void *argtable[] = { At, Amodel, Aiters, Ahidden, Alr, Ahelp, Aend };
  int nerrors = arg_parse(argc, argv, argtable);
  // special flags
  if (Ahelp->count > 0) {
    usage(argtable, progname);
    exit(0);
  }
  if (nerrors > 0) {
    arg_print_errors(stderr, Aend, progname);
    fprintf(stderr, "Try '%s --help' for more information.\n", progname);
    exit(EXIT_FAILURE);
  }

  std::cout << "=== Loading data ===" << std::endl;
  FANN::training_data data;
  if (data.read_train_from_file(At->filename[0])) {
    std::cout << "loaded " << data.length_train_data() << " training examples" << std::endl;
    data.shuffle_train_data();

    const float learning_rate = Alr->dval[0];
    const unsigned int num_layers = 3;
    const unsigned int num_input = data.num_input_train_data();
    const unsigned int num_output = data.num_output_train_data();
    const unsigned int num_hidden = Ahidden->ival[0];
    const float desired_error = 0.001f;
    const unsigned int max_iterations = Aiters->ival[0];
    const unsigned int iterations_between_reports = 10;

    std::cout << "=== Creating network ===" << std::endl;
    std::cout << "Input   nodes: " <<  num_input << std::endl;
    std::cout << "Hiidden nodes: " <<  num_hidden << std::endl;
    std::cout << "Output  nodes: " <<  num_output << std::endl;

    FANN::neural_net net;
    net.create_standard(num_layers, num_input, num_hidden, num_output);
    net.set_learning_rate(learning_rate);

    //net.set_training_algorithm(FANN::TRAIN_INCREMENTAL  );
    net.set_training_algorithm(FANN::TRAIN_QUICKPROP  );
    //net.set_training_algorithm(FANN::TRAIN_BATCH);
    //net.set_training_algorithm(FANN::TRAIN_RPROP);

    net.set_activation_steepness_output(1.0);
    net.set_activation_steepness_hidden(1.0);

    net.set_activation_function_hidden(FANN::SIGMOID_SYMMETRIC);
    net.set_activation_function_output(FANN::SIGMOID);

    net.init_weights(data);

    std::cout << "=== Training network ===" << std::endl;
    net.train_on_data(data, max_iterations, iterations_between_reports, desired_error);
    std::cout << std::endl << "Saving network to" << std::string(Amodel->filename[0]) << std::endl;

    net.save(Amodel->filename[0]);
  }

  return 0;
}
