/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// WARNING: this code was copied automatically from
// https://github.com/dguest/lwtnn.git (rev v1.0-55-gab82001)
// Please don't edit it! To get the latest version, run
// > ./update-lwtnn.sh
// from JetTagTools/share

#ifndef LIGHTWEIGHT_NEURAL_NETWORK_HH
#define LIGHTWEIGHT_NEURAL_NETWORK_HH

// Lightweight Tagger
//
// This is a simple NN implementation, designed to be lightweight in
// terms of both size and dependencies. For sanity we use Eigen, but
// otherwise this aims to be a minimal NN class which is fully
// configurable at runtime.
//
// Author: Dan Guest <dguest@cern.ch>

#include "NNLayerConfig.h"

#include <Eigen/Dense>

#include <vector>
#include <stdexcept>
#include <map>
#include <functional>

namespace lwt {

  using Eigen::VectorXd;
  using Eigen::MatrixXd;

  // use a normal map externally, since these are more common in user
  // code.  TODO: is it worth changing to unordered_map?
  typedef std::map<std::string, double> ValueMap;
  typedef std::vector<std::pair<std::string, double> > ValueVector;

  // _______________________________________________________________________
  // layer classes

  class ILayer
  {
  public:
    virtual ~ILayer() {}
    virtual VectorXd compute(const VectorXd&) const = 0;
  };

  class DummyLayer: public ILayer
  {
  public:
    virtual VectorXd compute(const VectorXd&) const;
  };

  class UnaryActivationLayer: public ILayer
  {
  public:
    UnaryActivationLayer(Activation);
    virtual VectorXd compute(const VectorXd&) const;
  private:
    std::function<double(double)> _func;
  };

  class SoftmaxLayer: public ILayer
  {
  public:
    virtual VectorXd compute(const VectorXd&) const;
  };

  class BiasLayer: public ILayer
  {
  public:
    BiasLayer(const VectorXd& bias);
    BiasLayer(const std::vector<double>& bias);
    virtual VectorXd compute(const VectorXd&) const;
  private:
    VectorXd _bias;
  };

  class MatrixLayer: public ILayer
  {
  public:
    MatrixLayer(const MatrixXd& matrix);
    virtual VectorXd compute(const VectorXd&) const;
  private:
    MatrixXd _matrix;
  };

  class MaxoutLayer: public ILayer
  {
  public:
    typedef std::pair<MatrixXd, VectorXd> InitUnit;
    MaxoutLayer(const std::vector<InitUnit>& maxout_tensor);
    virtual VectorXd compute(const VectorXd&) const;
  private:
    std::vector<MatrixXd> _matrices;
    MatrixXd _bias;
  };

  class DenseLayer: public ILayer
  {
  public:
    DenseLayer(const MatrixXd& matrix,
               const VectorXd& bias,
               Activation activation);
    virtual VectorXd compute(const VectorXd&) const;
  private:
    MatrixXd _matrix;
    VectorXd _bias;
    std::function<double(double)> _activation;
  };

  //http://arxiv.org/pdf/1505.00387v2.pdf
  class HighwayLayer: public ILayer
  {
  public:
    HighwayLayer(const MatrixXd& W,
                 const VectorXd& b,
                 const MatrixXd& W_carry,
                 const VectorXd& b_carry,
                 Activation activation);
    virtual VectorXd compute(const VectorXd&) const;
  private:
    MatrixXd _w_t;
    VectorXd _b_t;
    MatrixXd _w_c;
    VectorXd _b_c;
    std::function<double(double)> _act;
  };

  // ______________________________________________________________________
  // the NN class

  class Stack
  {
  public:
    // constructor for dummy net
    Stack();
    // constructor for real net
    Stack(size_t n_inputs, const std::vector<LayerConfig>& layers,
          size_t skip_layers = 0);
    ~Stack();

    // make non-copyable for now
    Stack(Stack&) = delete;
    Stack& operator=(Stack&) = delete;

    VectorXd compute(VectorXd) const;
    size_t n_outputs() const;

  private:
    // returns the size of the next layer
    size_t add_layers(size_t n_inputs, const LayerConfig&);
    size_t add_dense_layers(size_t n_inputs, const LayerConfig&);
    size_t add_highway_layers(size_t n_inputs, const LayerConfig&);
    size_t add_maxout_layers(size_t n_inputs, const LayerConfig&);
    std::vector<ILayer*> _layers;
    size_t _n_outputs;
  };

  // ______________________________________________________________________
  // input preprocessor (handles normalization and packing into Eigen)

  class InputPreprocessor
  {
  public:
    InputPreprocessor(const std::vector<Input>& inputs);
    VectorXd operator()(const ValueMap&) const;
  private:
    // input transformations
    VectorXd _offsets;
    VectorXd _scales;
    std::vector<std::string> _names;
  };

  // ______________________________________________________________________
  // high-level wrapper

  class LightweightNeuralNetwork
  {
  public:
    LightweightNeuralNetwork(const std::vector<Input>& inputs,
                             const std::vector<LayerConfig>& layers,
                             const std::vector<std::string>& outputs);
    // disable copying until we need it...
    LightweightNeuralNetwork(LightweightNeuralNetwork&) = delete;
    LightweightNeuralNetwork& operator=(LightweightNeuralNetwork&) = delete;

    // use a normal map externally, since these are more common in
    // user code.
    // TODO: is it worth changing to unordered_map?
    ValueMap compute(const ValueMap&) const;

  private:
    // use the Stack class above as the computational core
    Stack _stack;
    InputPreprocessor _preproc;

    // output labels
    std::vector<std::string> _outputs;

  };

  // ______________________________________________________________________
  // Activation functions

  // note that others are supported but are too simple to
  // require a special function
  double nn_sigmoid( double x );
  double nn_hard_sigmoid( double x );
  double nn_tanh( double x );
  double nn_relu( double x );
  std::function<double(double)> get_activation(lwt::Activation);

  // WARNING: you own this pointer! Only call when assigning to member data!
  ILayer* get_raw_activation_layer(Activation);

  // ______________________________________________________________________
  // utility functions

  // functions to build up basic units from vectors
  MatrixXd build_matrix(const std::vector<double>& weights, size_t n_inputs);
  VectorXd build_vector(const std::vector<double>& bias);

  // consistency checks
  void throw_if_not_maxout(const LayerConfig& layer);
  void throw_if_not_dense(const LayerConfig& layer);

  // LSTM component for convenience in some layers
  struct DenseComponents
  {
    Eigen::MatrixXd W;
    Eigen::MatrixXd U;
    Eigen::VectorXd b;
  };
  DenseComponents get_component(const lwt::LayerConfig& layer, size_t n_in);

  // ______________________________________________________________________
  // exceptions

  // base exception
  class LightweightNNException: public std::logic_error {
  public:
    LightweightNNException(std::string problem);
  };

  // thrown by the constructor if something goes wrong
  class NNConfigurationException: public LightweightNNException {
  public:
    NNConfigurationException(std::string problem);
  };

  // thrown by `compute`
  class NNEvaluationException: public LightweightNNException {
  public:
    NNEvaluationException(std::string problem);
  };
}
#endif
