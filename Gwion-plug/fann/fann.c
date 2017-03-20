// TODO
//    enum
//    check
//    test


#include <fann.h>
//#include <doublefann.h>

#include <fann_data.h>

#include "type.h"
#include "dl.h"
#include "import.h"
#include "err_msg.h"
#include "lang.h"

static struct Type_ t_fann_connect = { "FANN_connect", sizeof(m_uint), &t_object };
static struct Type_ t_fann_base =    { "FANN_base",    sizeof(m_uint), &t_object };
static struct Type_ t_fann =         { "FANN",         sizeof(m_uint), &t_fann_base };
static struct Type_ t_fann_data =    { "FANN_data",    sizeof(m_uint), &t_fann_base };

static m_int o_fann_error, o_fann_from, o_fann_to, o_fann_weight;

#define ERROR(o) *(struct fann_error**)(o->d.data + o_fann_error)
#define FANN(o)  *(struct fann **)(o->d.data + o_fann_error)
#define DATA(o)  *(struct fann_train_data **)(o->d.data + o_fann_error)

static DTOR(fann_dtor)
{
  if(FANN(o))
    fann_destroy(FANN(o));
}

static DTOR(data_dtor)
{
  if(DATA(o))
    fann_destroy_train(DATA(o));
}

static MFUN(error_log)
{
  FILE* file = IO_FILE(*(M_Object*)(shred->mem + SZ_INT));
  fann_set_error_log(ERROR(o), file);
}

static MFUN(errno)
{
  RETURN->d.v_uint = fann_get_errno(ERROR(o));
}

static MFUN(reset_errno)
{
  fann_reset_errno(ERROR(o));
}

static MFUN(errstr)
{
  m_str str = fann_get_errstr(ERROR(o));
  RETURN->d.v_uint = (m_uint)new_String(str ? str : "no error");
}

static MFUN(reset_errstr)
{
  fann_reset_errstr(ERROR(o));
}

static MFUN(print_error)
{
  fann_print_error(ERROR(o));
}

static MFUN(standard)
{
  m_uint i, size;
  m_uint num_layers = *(m_uint*)(shred->mem + SZ_INT);
  M_Object array    = *(M_Object*)(shred->mem + SZ_INT*2);
  if(FANN(o))
    fann_destroy(FANN(o));
  size = m_vector_size(array->d.array);
/*  unsigned int ptr[size];*/
  unsigned int ptr[size];
  for(i = 0; i < size; i++)
    ptr[i] = i_vector_at(array->d.array, i);
  FANN(o) = fann_create_standard_array(num_layers, ptr);
}

static MFUN(shortcut)
{
  m_uint i, size;
  m_uint num_layers = *(m_uint*)(shred->mem + SZ_INT);
  M_Object array    = *(M_Object*)(shred->mem + SZ_INT*2);
  if(FANN(o))
    fann_destroy(FANN(o));
  size = m_vector_size(array->d.array);
  unsigned int ptr[size];
  for(i = 0; i < size; i++)
    ptr[i] = i_vector_at(array->d.array, i);
  FANN(o) = fann_create_shortcut_array(num_layers, ptr);
}

static MFUN(sparse)
{
  m_uint i, size;
  m_float rate      = *(m_float*)(shred->mem + SZ_INT);
  m_uint num_layers = *(m_uint*)(shred->mem + SZ_INT + SZ_FLOAT);
  M_Object array    = *(M_Object*)(shred->mem + SZ_FLOAT + SZ_INT*2);
  if(FANN(o))
    fann_destroy(FANN(o));
  size = m_vector_size(array->d.array);
  unsigned int ptr[size];
  for(i = 0; i < size; i++)
    ptr[i] = i_vector_at(array->d.array, i);
  FANN(o) = fann_create_sparse_array(rate, num_layers, ptr);
}

static MFUN(randomize)
{
  if(!FANN(o))
    return;
  fann_randomize_weights(FANN(o), *(m_float*)(shred->mem + SZ_INT), *(m_float*)(shred->mem + SZ_INT + SZ_FLOAT));
}

static MFUN(init)
{
  if(!FANN(o))
    return;
  fann_init_weights(FANN(o), DATA(o));
}

static MFUN(print_connections)
{
  if(!FANN(o))
    return;
  fann_print_connections(FANN(o));
}

// parameters
static MFUN(parameters)
{
  if(!FANN(o))
    return;
  fann_print_parameters(FANN(o));
}

static MFUN(input)
{
  RETURN->d.v_uint = FANN(o) ? fann_get_num_input(FANN(o)) : - 1;
}

static MFUN(output)
{
  RETURN->d.v_uint = FANN(o) ? fann_get_num_output(FANN(o)) : -1;
}

static MFUN(neurons)
{
  RETURN->d.v_uint = FANN(o) ? fann_get_total_neurons(FANN(o)) : -1;
}

static MFUN(connections)
{
  RETURN->d.v_uint = FANN(o) ? fann_get_total_connections(FANN(o)) : -1;
}

static MFUN(type)
{
  RETURN->d.v_uint = FANN(o) ? fann_get_network_type(FANN(o)) : -1;
}

static MFUN(rate)
{
  RETURN->d.v_float = FANN(o) ? fann_get_connection_rate(FANN(o)) : -1;
}

static MFUN(layer)
{
  RETURN->d.v_uint = FANN(o) ? fann_get_num_layers(FANN(o)) : -1;
}

static MFUN(layers)
{
  if(!FANN(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  m_uint i, size = fann_get_num_layers(FANN(o));
  M_Object ret = new_M_Array(SZ_INT, size, 1);
  unsigned int j[size];
  fann_get_layer_array(FANN(o), j);
  for(i = 0; i < size; i++)
    i_vector_set(ret->d.array, i, j[i]);
  RETURN->d.v_uint = (m_uint)ret;
}
static MFUN(bias)
{
  if(!FANN(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  m_uint i, size = fann_get_num_layers(FANN(o));
  M_Object ret = new_M_Array(SZ_INT, size, 1);
  unsigned int j[size];
  fann_get_bias_array(FANN(o), j);
  for(i = 0; i < size; i++)
    i_vector_set(ret->d.array, i, j[i]);
  RETURN->d.v_uint = (m_uint)ret;
}

static struct fann_connection to_fann(M_Object o)
{
  struct fann_connection c;
  c.from_neuron = *(m_uint*)(o->d.data + o_fann_from);
  c.to_neuron   = *(m_uint*)(o->d.data + o_fann_to);
  c.weight      = *(m_float*)(o->d.data + o_fann_weight);
  return c;
}
static M_Object from_fann(struct fann_connection c)
{
  M_Object o= new_M_Object();
  initialize_object(o, &t_fann_connect);
  *(m_uint*)(o->d.data + o_fann_from)    = c.from_neuron;
  *(m_uint*)(o->d.data + o_fann_to)      = c.to_neuron;
  *(m_float*)(o->d.data + o_fann_weight) = c.weight;
  return o;
}
static MFUN(connection_array)
{
  if(!FANN(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  m_uint i, size = fann_get_total_connections(FANN(o));
  M_Object ret = new_M_Array(SZ_INT, size, 1);
  struct fann_connection c[size];
  fann_get_connection_array(FANN(o), c);
  for(i= 0; i < size; i++)
    i_vector_set(ret->d.array, i, (m_uint)from_fann(c[i]));
  RETURN->d.v_uint = (m_uint)ret;
}
static MFUN(weigth_array)
{
  if(!FANN(o))
    return;
  M_Object obj = *(M_Object*)(shred->mem + SZ_INT);
  m_uint i, size = m_vector_size(obj->d.array);
  struct fann_connection c[size];
  for(i = 0; i < size; i++)
    c[i] = to_fann((M_Object)i_vector_at(obj->d.array, i));
  fann_set_weight_array(FANN(o), c, size);
}

static MFUN(weigth)
{
  m_int   from   = *(m_int*)(shred->mem + SZ_INT);
  m_int   to = *(m_int*)(shred->mem + SZ_INT*2);
  m_float f    = *(m_float*)(shred->mem + SZ_INT*3);
  fann_set_weight(FANN(o), from, to, f);
}

static MFUN(get_weigths)
{
  if(!FANN(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  m_uint i, size = fann_get_total_connections(FANN(o));
  M_Object ret = new_M_Array(SZ_FLOAT, size, 1);
  m_float f[size];
  fann_get_weights(FANN(o), f);
  for(i = 0; i < size; i++)
    f_vector_set(ret->d.array, i, f[i]);
  RETURN->d.v_uint = (m_uint)ret;
}

static MFUN(set_weigths)
{
  if(!FANN(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  m_uint i, size = fann_get_total_connections(FANN(o));
  M_Object ret = *(M_Object*)(shred->mem + SZ_INT);
  if(m_vector_size(ret->d.array) < size)
  if(!FANN(o))
  {
    err_msg(INSTR_, 0, "invalid array size for weights (%i). should be %i", m_vector_size(ret->d.array), size);
    RETURN->d.v_uint = 0;
    return;
  }
  m_float f[size];
  for(i = 0; i < size; i++)
    f[i] = f_vector_at(ret->d.array, i);
  fann_set_weights(FANN(o), f);
  RETURN->d.v_uint = (m_uint)ret;
}
// get/set user data
static SFUN(disable_seed)
{
  fann_disable_seed_rand();
}

static SFUN(enable_seed)
{
  fann_enable_seed_rand();
}

/*
// FIXED POINT only

MFUN(decimal_point)
{
  RETURN->d.v_uint = fann_get_decimal_point(FANN(o));
}

MFUN(multiplier)
{
  RETURN->d.v_uint = fann_get_multiplier(FANN(o));
}
*/
static SFUN(type_str)
{
  m_int i = *(m_int*)(shred->mem + SZ_INT);
  if(i < 0 || i > 1)
  {
    RETURN->d.v_uint = 0;
    return;
  }
  RETURN->d.v_uint = (m_uint)new_String((m_str)FANN_NETTYPE_NAMES[i]);
}

static MFUN(load)
{
  FANN(o) = fann_create_from_file(STRING(shred->mem + SZ_INT));
}

static MFUN(save)
{
  fann_save(FANN(o), STRING(shred->mem + SZ_INT));
}
// training
static MFUN(train)
{
  m_uint i;
  M_Object o_in  = *(M_Object*)(shred->mem + SZ_INT);
  M_Object o_out = *(M_Object*)(shred->mem + SZ_INT*2);
  m_uint   s_in  = m_vector_size(o_in->d.array);
  m_uint   s_out  = m_vector_size(o_out->d.array);
  m_float in[s_in], out[s_out];
  for(i = 0; i < s_in; i++)
    in[i] = f_vector_at(o_in->d.array, i);
  for(i = 0; i < s_out; i++)
    in[i] = f_vector_at(o_out->d.array, i);
  fann_train(FANN(o), in, out);
}

static MFUN(test)
{
  m_uint i;
  M_Object o_in  = *(M_Object*)(shred->mem + SZ_INT);
  M_Object o_out = *(M_Object*)(shred->mem + SZ_INT*2);
  m_uint   s_in  = m_vector_size(o_in->d.array);
  m_uint   s_out = m_vector_size(o_out->d.array);
  m_uint s_ret   = fann_get_num_output(FANN(o));
  m_float in[s_in], out[s_out];
  for(i = 0; i < s_in; i++)
    in[i] = f_vector_at(o_in->d.array, i);
  for(i = 0; i < s_out; i++)
    in[i] = f_vector_at(o_out->d.array, i);
  m_float* f = fann_test(FANN(o), in, out);
  M_Object ret = new_M_Array(SZ_FLOAT, s_ret, 1);
  for(i = 0; i < s_ret; i++)
    f_vector_set(o_out->d.array, i, f[i]);
  RETURN->d.v_uint = (m_uint)ret;
}

static MFUN(get_MSE)
{
  RETURN->d.v_float = fann_get_MSE(FANN(o));
}

static MFUN(get_bit_fail)
{
  RETURN->d.v_uint = fann_get_bit_fail(FANN(o));
}

static MFUN(reset_MSE)
{
  fann_reset_MSE(FANN(o));
}

static MFUN(run)
{
  m_uint i, size;
  if(!FANN(o) || !DATA(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  M_Object array    = *(M_Object*)(shred->mem + SZ_INT);
  size = m_vector_size(array->d.array);
  m_float  ptr[size];
  for(i = 0; i < size; i++)
    ptr[i] = i_vector_at(array->d.array, i);
  M_Object ret = new_M_Array(SZ_FLOAT, fann_get_num_output(FANN(o)), 1);
  m_float *f = fann_run(FANN(o), ptr);
  for(i = 0; i < fann_get_num_output(FANN(o)); i++)
    f_vector_set(ret->d.array, i, f[i]);
  RETURN->d.v_uint = (m_uint)ret;
}

// Training Data Manipulation
static MFUN(scale_train)
{
  M_Object data   = *(M_Object*)(shred->mem + SZ_INT);
  fann_scale_train(FANN(o),DATA(data));
}

static MFUN(descale_train)
{
  M_Object data   = *(M_Object*)(shred->mem + SZ_INT);
  fann_descale_train(FANN(o),DATA(data));
}

static MFUN(input_scaling_params)
{
  M_Object data  = *(M_Object*)(shred->mem + SZ_INT);
  m_float min    = *(m_float*)(shred->mem + SZ_INT*2);
  m_float max    = *(m_float*)(shred->mem + SZ_INT*2 + SZ_FLOAT);
  RETURN->d.v_uint = fann_set_input_scaling_params(FANN(o), DATA(data), min, max);
}

static MFUN(output_scaling_params)
{
  M_Object data  = *(M_Object*)(shred->mem + SZ_INT);
  m_float min    = *(m_float*)(shred->mem + SZ_INT*2);
  m_float max    = *(m_float*)(shred->mem + SZ_INT*2 + SZ_FLOAT);
  RETURN->d.v_uint = fann_set_output_scaling_params(FANN(o), DATA(data), min, max);
}

static MFUN(scaling_params)
{
  M_Object data  = *(M_Object*)(shred->mem + SZ_INT);
  m_float imin   = *(m_float*)(shred->mem + SZ_INT*2);
  m_float imax   = *(m_float*)(shred->mem + SZ_INT*2 + SZ_FLOAT);
  m_float omin   = *(m_float*)(shred->mem + SZ_INT*2 + SZ_FLOAT*2);
  m_float omax   = *(m_float*)(shred->mem + SZ_INT*2 + SZ_FLOAT*3);
  RETURN->d.v_uint = fann_set_scaling_params(FANN(o), DATA(data), imin, imax, omin, omax);
}
static MFUN(clear_scaling_params)
{
  RETURN->d.v_uint = fann_clear_scaling_params(FANN(o));
}

static MFUN(scale_input)
{
  if(!FANN(o))
    return;
  M_Object obj = *(M_Object*)(shred->mem + SZ_INT);
  m_uint i, size = m_vector_size(obj->d.array);
  m_float f[size];
  for(i = 0; i < size; i++)
    f[i] = f_vector_at(obj->d.array, i);
  fann_scale_input(FANN(o), f);
}

static MFUN(scale_output)
{
  if(!FANN(o))
    return;
  M_Object obj = *(M_Object*)(shred->mem + SZ_INT);
  m_uint i, size = m_vector_size(obj->d.array);
  m_float f[size];
  for(i = 0; i < size; i++)
    f[i] = f_vector_at(obj->d.array, i);
  fann_scale_output(FANN(o), f);
}

static MFUN(descale_input)
{
  if(!FANN(o))
    return;
  M_Object obj = *(M_Object*)(shred->mem + SZ_INT);
  m_uint i, size = m_vector_size(obj->d.array);
  m_float f[size];
  for(i = 0; i < size; i++)
    f[i] = f_vector_at(obj->d.array, i);
  fann_descale_input(FANN(o), f);
  for(i = 0; i < size; i++)
    f_vector_set(obj->d.array, i, f[i]);
}

static MFUN(descale_output)
{
  if(!FANN(o))
    return;
  M_Object obj = *(M_Object*)(shred->mem + SZ_INT);
  m_uint i, size = m_vector_size(obj->d.array);
  m_float f[size];
  for(i = 0; i < size; i++)
    f[i] = f_vector_at(obj->d.array, i);
  fann_descale_output(FANN(o), f);
  for(i = 0; i < size; i++)
    f_vector_set(obj->d.array, i, f[i]);
}

// Training Data Training
static MFUN(train_on_data)
{
  M_Object data   = *(M_Object*)(shred->mem + SZ_INT);
  m_uint   epoch  = *(m_uint*)(shred->mem + SZ_INT*2);
  m_uint   report = *(m_uint*)(shred->mem + SZ_INT*3);
  m_float  error  = *(m_uint*)(shred->mem + SZ_INT*4);
  fann_train_on_data(FANN(o), DATA(data), epoch, report, error);
}

static MFUN(train_on_file)
{
  M_Object data   = *(M_Object*)(shred->mem + SZ_INT);
  m_uint   epoch  = *(m_uint*)(shred->mem + SZ_INT*2);
  m_uint   report = *(m_uint*)(shred->mem + SZ_INT*3);
  m_float  error  = *(m_uint*)(shred->mem + SZ_INT*4);
  fann_train_on_file(FANN(o), STRING(data), epoch, report, error);
}

static MFUN(train_epoch)
{
  M_Object data = *(M_Object*)(shred->mem + SZ_INT);
  RETURN->d.v_float = fann_train_epoch(FANN(o), DATA(data));
}

static MFUN(test_data)
{
  M_Object data = *(M_Object*)(shred->mem + SZ_INT);
  RETURN->d.v_float = fann_test_data(FANN(o), DATA(data));
}
// Training Data Manipulation
static MFUN(train_from_file)
{
  if(DATA(o))
    fann_destroy_train(DATA(o));
  fann_read_train_from_file(STRING(*(M_Object*)(shred->mem + SZ_INT)));
}

static MFUN(train_from_array)
{
  if(DATA(o))
    fann_destroy_train(DATA(o));
  m_uint i, j, y, in_size, out_size;
  m_uint  num_data = *(m_uint*)(shred->mem + SZ_INT);
  M_Object obj;
  M_Object  in_obj = *(M_Object*)(shred->mem + SZ_INT*2);
  M_Object out_obj = *(M_Object*)(shred->mem + SZ_INT*3);
  in_size  = m_vector_size(in_obj->d.array);
  m_float* in[in_size];
  for(i = 0; i < in_size; i++)
  {
    obj = (M_Object)i_vector_at(in_obj->d.array, i);
    y = m_vector_size(obj->d.array);
    m_float f[y];
    in[i] = f;
    for(j = 0; j < y; j++)
      f[j] = f_vector_at(obj->d.array, j);
  }
  out_size = m_vector_size(out_obj->d.array);
  m_float* out[out_size];
  for(i = 0; i < out_size; i++)
  {
    obj = (M_Object)i_vector_at(out_obj->d.array, i);
    y = m_vector_size(obj->d.array);
    m_float f[y];
    out[i] = f;
    for(j = 0; j < y; j++)
      f[j] = f_vector_at(obj->d.array, j);
  }
  DATA(o) = fann_create_train_pointer_array(num_data, in_size, in, out_size, out);
}

static MFUN(train_create)
{
  if(DATA(o))
    fann_destroy_train(DATA(o));
  DATA(o) = fann_create_train(*(m_uint*)(shred->mem + SZ_INT),  *(m_uint*)(shred->mem + SZ_INT*2), *(m_uint*)(shred->mem + SZ_INT*3));
}

static MFUN(train_input)
{
  if(!DATA(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  m_float* f = fann_get_train_input(DATA(o), *(m_uint*)(shred->mem + SZ_INT));
  m_uint i, size = sizeof(f)/sizeof(m_float);
  M_Object ret = new_M_Array(SZ_FLOAT, size, 1);
  for(i = 0; i < size; i++)
    f_vector_set(ret->d.array, i, f[i]);
  RETURN->d.v_uint = (m_uint)ret;
}

static MFUN(train_output)
{
  if(!DATA(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  m_float* f = fann_get_train_output(DATA(o), *(m_uint*)(shred->mem + SZ_INT));
  m_uint i, size = sizeof(f)/sizeof(m_float);
  M_Object ret = new_M_Array(SZ_FLOAT, size, 1);
  for(i = 0; i < size; i++)
    f_vector_set(ret->d.array, i, f[i]);
  RETURN->d.v_uint = (m_uint)ret;
}

static MFUN(train_shuffle)
{
  if(!DATA(o))
    return;
  fann_shuffle_train_data(DATA(o));
}

static MFUN(train_min_in)
{
  if(!DATA(o))
  {
    RETURN->d.v_float = NAN;
    return;
  }
  RETURN->d.v_float = fann_get_min_train_input(DATA(o));
}

static MFUN(train_max_in)
{
  if(!DATA(o))
  {
    RETURN->d.v_float = NAN;
    return;
  }
  RETURN->d.v_float = fann_get_max_train_input(DATA(o));
}

static MFUN(train_min_out)
{
  if(!DATA(o))
  {
    RETURN->d.v_float = NAN;
    return;
  }
  RETURN->d.v_float = fann_get_min_train_output(DATA(o));
}

static MFUN(train_max_out)
{
  if(!DATA(o))
  {
    RETURN->d.v_float = NAN;
    return;
  }
  RETURN->d.v_float = fann_get_max_train_output(DATA(o));
}

static MFUN(train_input_scale)
{
  m_float min    = *(m_float*)(shred->mem + SZ_INT);
  m_float max    = *(m_float*)(shred->mem + SZ_INT + SZ_FLOAT);
  fann_scale_input_train_data(DATA(o), min, max);
}

static MFUN(train_output_scale)
{
  m_float min    = *(m_float*)(shred->mem + SZ_INT);
  m_float max    = *(m_float*)(shred->mem + SZ_INT + SZ_FLOAT);
  fann_scale_output_train_data(DATA(o), min, max);
}

static MFUN(train_scale)
{
  m_float min    = *(m_float*)(shred->mem + SZ_INT);
  m_float max    = *(m_float*)(shred->mem + SZ_INT + SZ_FLOAT);
  fann_scale_train_data(DATA(o), min, max);
}

static MFUN(train_length)
{
  RETURN->d.v_uint = fann_length_train_data(DATA(o));
}

static MFUN(train_num_input)
{
  RETURN->d.v_uint = fann_num_input_train_data(DATA(o));
}

static MFUN(train_num_output)
{
  RETURN->d.v_uint = fann_num_input_train_data(DATA(o));
}

static MFUN(train_save)
{
  M_Object file = *(M_Object*)(shred->mem + SZ_INT);
  RETURN->d.v_uint = fann_save_train(DATA(o), STRING(file));
}

static SFUN(train_merge)
{
  M_Object ret = new_M_Object();
  M_Object l = *(M_Object*)(shred->mem + SZ_INT);
  M_Object r = *(M_Object*)(shred->mem + SZ_INT*2);
  initialize_object(ret, &t_fann_data);
  DATA(ret) = fann_merge_train_data(DATA(l), DATA(r));
  RETURN->d.v_uint = (m_uint)ret;
}

static SFUN(train_duplicate)
{
  M_Object ret = new_M_Object();
  M_Object l = *(M_Object*)(shred->mem + SZ_INT);
  M_Object r = *(M_Object*)(shred->mem + SZ_INT*2);
  initialize_object(ret, &t_fann_data);
  DATA(ret) = fann_duplicate_train_data(DATA(l));
  RETURN->d.v_uint = (m_uint)ret;
}

static MFUN(train_do_subset)
{
  M_Object ret = new_M_Object();
  m_uint pos = *(m_uint*)(shred->mem + SZ_INT);
  m_uint len = *(m_uint*)(shred->mem + SZ_INT*2);
  initialize_object(ret, &t_fann_data);
  DATA(ret) = fann_subset_train_data(DATA(o), pos, len);
  RETURN->d.v_uint = (m_uint)ret;
}

// fann parameters
MFUN(get_training_algorithm)
{
  RETURN->d.v_uint = fann_get_training_algorithm(FANN(o));
}

MFUN(set_training_algorithm)
{
  RETURN->d.v_uint = *(m_uint*)(shred->mem + SZ_INT);
  fann_set_training_algorithm(FANN(o), RETURN->d.v_uint);
}

MFUN(get_learning_rate)
{
  RETURN->d.v_float = fann_get_learning_rate(FANN(o));
}

MFUN(set_learning_rate)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_learning_rate(FANN(o), RETURN->d.v_float);
}


MFUN(get_learning_momentum)
{
  RETURN->d.v_float = fann_get_learning_momentum(FANN(o));
}

MFUN(set_learning_momentum)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_learning_momentum(FANN(o), RETURN->d.v_float);
}

MFUN(get_activation_function)
{
  RETURN->d.v_uint = fann_get_activation_function(FANN(o), 
    *(m_int*)(shred->mem + SZ_INT), *(m_int*)(shred->mem + SZ_INT*2));
}

MFUN(set_activation_function)
{
  RETURN->d.v_uint = *(m_int*)(shred->mem + SZ_INT);
  fann_set_activation_function(FANN(o), RETURN->d.v_uint, 
    *(m_int*)(shred->mem + SZ_INT*2), *(m_int*)(shred->mem + SZ_INT*3));
}
MFUN(set_activation_function_layer)
{
  RETURN->d.v_uint = *(m_int*)(shred->mem + SZ_INT);
  fann_set_activation_function_layer(FANN(o), RETURN->d.v_uint,
    *(m_int*)(shred->mem + SZ_INT*2));
}

MFUN(set_activation_function_hidden)
{
  RETURN->d.v_uint = *(m_int*)(shred->mem + SZ_INT);
  fann_set_activation_function_hidden(FANN(o), RETURN->d.v_uint);
}

MFUN(set_activation_function_output)
{
  RETURN->d.v_uint = *(m_int*)(shred->mem + SZ_INT);
  fann_set_activation_function_output(FANN(o), RETURN->d.v_uint);
}

MFUN(get_activation_steepness)
{
  RETURN->d.v_uint = fann_get_activation_steepness(FANN(o),
    *(m_int*)(shred->mem + SZ_INT), *(m_int*)(shred->mem + SZ_INT*2));
}

MFUN(set_activation_steepness)
{
  RETURN->d.v_uint = *(m_int*)(shred->mem + SZ_INT);
  fann_set_activation_steepness(FANN(o), RETURN->d.v_uint,
    *(m_int*)(shred->mem + SZ_INT*2), *(m_int*)(shred->mem + SZ_INT*3));
}
MFUN(set_activation_steepness_layer)
{
  RETURN->d.v_uint = *(m_int*)(shred->mem + SZ_INT);
  fann_set_activation_steepness_layer(FANN(o), RETURN->d.v_uint,
    *(m_int*)(shred->mem + SZ_INT*2));
}

MFUN(set_activation_steepness_hidden)
{
  RETURN->d.v_uint = *(m_int*)(shred->mem + SZ_INT);
  fann_set_activation_steepness_hidden(FANN(o), RETURN->d.v_uint);
}

MFUN(set_activation_steepness_output)
{
  RETURN->d.v_uint = *(m_int*)(shred->mem + SZ_INT);
  fann_set_activation_steepness_output(FANN(o), RETURN->d.v_uint);
}

MFUN(get_train_error_function)
{
  RETURN->d.v_uint = fann_get_train_error_function(FANN(o));
}

MFUN(set_train_error_function)
{
  RETURN->d.v_uint = *(m_uint*)(shred->mem + SZ_INT);
  fann_set_train_error_function(FANN(o), RETURN->d.v_uint);
}

MFUN(get_train_stop_function)
{
  RETURN->d.v_uint = fann_get_train_stop_function(FANN(o));
}

MFUN(set_train_stop_function)
{
  RETURN->d.v_uint = *(m_uint*)(shred->mem + SZ_INT);
  fann_set_train_stop_function(FANN(o), RETURN->d.v_uint);
}

MFUN(get_bit_fail_limit)
{
  RETURN->d.v_float = fann_get_bit_fail_limit(FANN(o));
}

MFUN(set_bit_fail_limit)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_bit_fail_limit(FANN(o), RETURN->d.v_uint);
}
// callback

MFUN(get_quickprop_decay)
{
  RETURN->d.v_float = fann_get_quickprop_decay(FANN(o));
}

MFUN(set_quickprop_decay)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_quickprop_decay(FANN(o), RETURN->d.v_uint);
}

MFUN(get_quickprop_mu)
{
  RETURN->d.v_float = fann_get_quickprop_mu(FANN(o));
}

MFUN(set_quickprop_mu)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_quickprop_mu(FANN(o), RETURN->d.v_uint);
}

MFUN(get_rprop_increase_factor)
{
  RETURN->d.v_float = fann_get_rprop_increase_factor(FANN(o));
}

MFUN(set_rprop_increase_factor)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_rprop_increase_factor(FANN(o), RETURN->d.v_uint);
}

MFUN(get_rprop_decrease_factor)
{
  RETURN->d.v_float = fann_get_rprop_decrease_factor(FANN(o));
}

MFUN(set_rprop_decrease_factor)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_rprop_decrease_factor(FANN(o), RETURN->d.v_uint);
}

MFUN(get_rprop_delta_min)
{
  RETURN->d.v_float = fann_get_rprop_delta_min(FANN(o));
}

MFUN(set_rprop_delta_min)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_rprop_delta_min(FANN(o), RETURN->d.v_uint);
}

MFUN(get_rprop_delta_max)
{
  RETURN->d.v_float = fann_get_rprop_delta_max(FANN(o));
}

MFUN(set_rprop_delta_max)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_rprop_delta_max(FANN(o), RETURN->d.v_uint);
}

MFUN(get_rprop_delta_zero)
{
  RETURN->d.v_float = fann_get_rprop_delta_zero(FANN(o));
}

MFUN(set_rprop_delta_zero)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_rprop_delta_zero(FANN(o), RETURN->d.v_uint);
}

MFUN(get_sarprop_weight_decay_shift)
{
  RETURN->d.v_float = fann_get_sarprop_weight_decay_shift(FANN(o));
}

MFUN(set_sarprop_weight_decay_shift)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_sarprop_weight_decay_shift(FANN(o), RETURN->d.v_uint);
}

MFUN(get_sarprop_step_error_threshold_factor)
{
  RETURN->d.v_float = fann_get_sarprop_step_error_threshold_factor(FANN(o));
}

MFUN(set_sarprop_step_error_threshold_factor)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_sarprop_step_error_threshold_factor(FANN(o), RETURN->d.v_uint);
}

MFUN(get_sarprop_step_error_shift)
{
  RETURN->d.v_float = fann_get_sarprop_step_error_shift(FANN(o));
}

MFUN(set_sarprop_step_error_shift)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_sarprop_step_error_shift(FANN(o), RETURN->d.v_uint);
}

MFUN(get_sarprop_temperature)
{
  RETURN->d.v_float = fann_get_sarprop_temperature(FANN(o));
}

MFUN(set_sarprop_temperature)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_sarprop_temperature(FANN(o), RETURN->d.v_uint);
}

// cascade parameters
MFUN(get_cascade_output_change_fraction)
{
  RETURN->d.v_float = fann_get_cascade_output_change_fraction(FANN(o));
}

MFUN(set_cascade_output_change_fraction)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_cascade_output_change_fraction(FANN(o), RETURN->d.v_uint);
}


MFUN(get_cascade_output_stagnation_epochs)
{
  RETURN->d.v_uint = fann_get_cascade_output_stagnation_epochs(FANN(o));
}

MFUN(set_cascade_output_stagnation_epochs)
{
  RETURN->d.v_uint = *(m_uint*)(shred->mem + SZ_INT);
  fann_set_cascade_output_stagnation_epochs(FANN(o), RETURN->d.v_uint);
}

MFUN(get_cascade_candidate_change_fraction)
{
  RETURN->d.v_float = fann_get_cascade_candidate_change_fraction(FANN(o));
}

MFUN(set_cascade_candidate_change_fraction)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_cascade_candidate_change_fraction(FANN(o), RETURN->d.v_uint);
}

MFUN(get_cascade_candidate_stagnation_epochs)
{
  RETURN->d.v_uint = fann_get_cascade_candidate_stagnation_epochs(FANN(o));
}

MFUN(set_cascade_candidate_stagnation_epochs)
{
  RETURN->d.v_uint = *(m_uint*)(shred->mem + SZ_INT);
  fann_set_cascade_candidate_stagnation_epochs(FANN(o), RETURN->d.v_uint);
}

MFUN(get_cascade_weight_multiplier)
{
  RETURN->d.v_float = fann_get_cascade_weight_multiplier(FANN(o));
}

MFUN(set_cascade_weight_multiplier)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_cascade_weight_multiplier(FANN(o), RETURN->d.v_uint);
}

MFUN(get_cascade_candidate_limit)
{
  RETURN->d.v_float = fann_get_cascade_candidate_limit(FANN(o));
}

MFUN(set_cascade_candidate_limit)
{
  RETURN->d.v_float = *(m_float*)(shred->mem + SZ_INT);
  fann_set_cascade_candidate_limit(FANN(o), RETURN->d.v_uint);
}

MFUN(get_cascade_max_out_epochs)
{
  RETURN->d.v_uint = fann_get_cascade_max_out_epochs(FANN(o));
}

MFUN(set_cascade_max_out_epochs)
{
  RETURN->d.v_uint = *(m_uint*)(shred->mem + SZ_INT);
  fann_set_cascade_max_out_epochs(FANN(o), RETURN->d.v_uint);
}

MFUN(get_cascade_min_out_epochs)
{
  RETURN->d.v_uint = fann_get_cascade_min_out_epochs(FANN(o));
}

MFUN(set_cascade_min_out_epochs)
{
  RETURN->d.v_uint = *(m_uint*)(shred->mem + SZ_INT);
  fann_set_cascade_min_out_epochs(FANN(o), RETURN->d.v_uint);
}

MFUN(get_cascade_max_cand_epochs)
{
  RETURN->d.v_uint = fann_get_cascade_max_cand_epochs(FANN(o));
}

MFUN(set_cascade_max_cand_epochs)
{
  RETURN->d.v_uint = *(m_uint*)(shred->mem + SZ_INT);
  fann_set_cascade_max_cand_epochs(FANN(o), RETURN->d.v_uint);
}

MFUN(get_cascade_min_cand_epochs)
{
  RETURN->d.v_uint = fann_get_cascade_min_cand_epochs(FANN(o));
}

MFUN(set_cascade_min_cand_epochs)
{
  RETURN->d.v_uint = *(m_uint*)(shred->mem + SZ_INT);
  fann_set_cascade_min_cand_epochs(FANN(o), RETURN->d.v_uint);
}

MFUN(get_cascade_activation_functions_count)
{
  RETURN->d.v_uint = fann_get_cascade_activation_functions_count(FANN(o));
}

MFUN(get_cascade_activation_functions)
{
  if(!FANN(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  m_uint i, size = fann_get_cascade_activation_functions_count(FANN(o));
  enum fann_activationfunc_enum * tmp = fann_get_cascade_activation_functions(FANN(o));
  M_Object ret = new_M_Array(SZ_INT, size, 1);
  for(i=0; i < size; i++)
    i_vector_set(ret->d.array, i, tmp[i]);
  RETURN->d.v_uint = (m_uint)ret;
}

MFUN(set_cascade_activation_functions)
{
  if(!FANN(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  M_Object ret = *(M_Object*)(shred->mem + SZ_INT);
  m_uint index = *(m_uint*)(shred->mem + SZ_INT*2);
  m_uint i, size = m_vector_size(ret->d.array);
  enum fann_activationfunc_enum tmp[size];
  for(i=0; i < size; i++)
    tmp[i] = i_vector_at(ret->d.array, i);
  fann_set_cascade_activation_functions(FANN(o), tmp, size);
  RETURN->d.v_uint = (m_uint)ret;
}

MFUN(get_cascade_activation_steepnesses_count)
{
  RETURN->d.v_uint = fann_get_cascade_activation_steepnesses_count(FANN(o));
}

MFUN(get_cascade_activation_steepnesses)
{
  if(!FANN(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  m_uint i, size = fann_get_cascade_activation_steepnesses_count(FANN(o));
  m_float* tmp = fann_get_cascade_activation_steepnesses(FANN(o));
  M_Object ret = new_M_Array(SZ_INT, size, 1);
  for(i=0; i < size; i++)
    f_vector_set(ret->d.array, i, tmp[i]);
  RETURN->d.v_uint = (m_uint)ret;
}

MFUN(set_cascade_activation_steepnesses)
{
  if(!FANN(o))
  {
    RETURN->d.v_uint = 0;
    return;
  }
  M_Object ret = *(M_Object*)(shred->mem + SZ_INT);
  m_uint index = *(m_uint*)(shred->mem + SZ_INT*2);
  m_uint i, size = m_vector_size(ret->d.array);
  m_float tmp[size];
  for(i=0; i < size; i++)
    tmp[i] = f_vector_at(ret->d.array, i);
  fann_set_cascade_activation_steepnesses(FANN(o), tmp, size);
  RETURN->d.v_uint = (m_uint)ret;
}

MFUN(get_cascade_num_candidate_groups)
{
  RETURN->d.v_uint = fann_get_cascade_num_candidate_groups(FANN(o));
}

MFUN(set_cascade_num_candidate_groups)
{
  RETURN->d.v_uint = *(m_uint*)(shred->mem + SZ_INT);
  fann_set_cascade_num_candidate_groups(FANN(o), RETURN->d.v_uint);
}

//IMPORT
m_bool import(Env env)
{
  DL_Func* fun;
  CHECK_BB(add_global_type(env, &t_fann_connect))
  CHECK_BB(import_class_begin(env, &t_fann_connect, env->global_nspc, NULL, NULL))
  o_fann_from = import_mvar(env, "int",  "from", 0, 0, "Unique number used to identify source neuron");
  CHECK_BB(o_fann_from)
  o_fann_to = import_mvar(env, "int",  "to", 0, 0, "Unique number used to identify destination neuron");
  CHECK_BB(o_fann_to)
  o_fann_weight = import_mvar(env, "int",  "weight", 0, 0, "The numerical value of the weight");
  CHECK_BB(o_fann_weight)
  CHECK_BB(import_class_end(env))

  // this is for error handling
  CHECK_BB(add_global_type(env, &t_fann_base))
  CHECK_BB(import_class_begin(env, &t_fann_base, env->global_nspc, NULL, NULL))
  o_fann_error = import_mvar(env, "int",  "@data", 0, 0, "train data");
  CHECK_BB(o_fann_error)
  fun = new_DL_Func("void", "log",  (m_uint)error_log);
    dl_func_add_arg(fun, "FileIO", "f");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "errno",  (m_uint)errno);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "reset_errno",  (m_uint)reset_errno);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "reset_errstr",  (m_uint)reset_errstr);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "errstr",  (m_uint)errstr);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "print_error",  (m_uint)print_error);
  CHECK_OB(import_mfun(env, fun))
  CHECK_BB(import_class_end(env))

//  Training Data Manipulation
  CHECK_BB(add_global_type(env, &t_fann_data))
  CHECK_BB(import_class_begin(env, &t_fann_data, env->global_nspc, NULL, data_dtor))
  fun = new_DL_Func("void", "from_data",  (m_uint)train_from_array);
    dl_func_add_arg(fun, "int", "num");
    dl_func_add_arg(fun, "float[][]", "in");
    dl_func_add_arg(fun, "float[][]", "out");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "load",  (m_uint)train_from_file);
    dl_func_add_arg(fun, "string", "filename");
    dl_func_add_arg(fun, "int", "epochs");
    dl_func_add_arg(fun, "int", "report");
    dl_func_add_arg(fun, "float", "error");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "save",  (m_uint)train_save);
    dl_func_add_arg(fun, "string", "filename");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "create",  (m_uint)train_create);
    dl_func_add_arg(fun, "int", "num_data");
    dl_func_add_arg(fun, "int", "num_in");
    dl_func_add_arg(fun, "int", "num_out");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float[]", "input",  (m_uint)train_input);
    dl_func_add_arg(fun, "int", "position");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float[]", "output",  (m_uint)train_output);
    dl_func_add_arg(fun, "int", "position");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "shuffle",  (m_uint)train_shuffle);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "min_in",  (m_uint)train_min_in);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "max_in",  (m_uint)train_max_in);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "max_in",  (m_uint)train_max_in);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "max_out",  (m_uint)train_max_out);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "length",  (m_uint)train_length);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "num_input",  (m_uint)train_num_input);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "num_ouput",  (m_uint)train_num_output);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("FANN_data", "subset",  (m_uint)train_do_subset);
    dl_func_add_arg(fun, "int", "pos");
    dl_func_add_arg(fun, "int", "len");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("FANN_data", "merge",  (m_uint)train_merge);
    dl_func_add_arg(fun, "FANN_data", "l");
    dl_func_add_arg(fun, "FANN_data", "r");
  CHECK_OB(import_sfun(env, fun))
  fun = new_DL_Func("FANN_data", "duplicate",  (m_uint)train_duplicate);
    dl_func_add_arg(fun, "FANN_data", "l");
  CHECK_OB(import_sfun(env, fun))

  CHECK_BB(import_class_end(env))

  CHECK_BB(add_global_type(env, &t_fann))
  CHECK_BB(import_class_begin(env, &t_fann, env->global_nspc, NULL, fann_dtor))
  fun = new_DL_Func("string", "type_str", (m_uint)type_str);
    dl_func_add_arg(fun, "int", "layer");
  CHECK_OB(import_sfun(env, fun))
  fun = new_DL_Func("void", "standard", (m_uint)standard);
    dl_func_add_arg(fun, "int", "layer");
    dl_func_add_arg(fun, "int[]", "outputs");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "shortcut",  (m_uint)shortcut);
    dl_func_add_arg(fun, "int", "layer");
    dl_func_add_arg(fun, "int[]", "outputs");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "sparse",   (m_uint)sparse);
    dl_func_add_arg(fun, "float", "rate");
    dl_func_add_arg(fun, "int", "layer");
    dl_func_add_arg(fun, "int[]", "outputs");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "randomize",  (m_uint)randomize);
    dl_func_add_arg(fun, "float", "min");
    dl_func_add_arg(fun, "float", "max");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "init",  (m_uint)init);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "print",  (m_uint)print_connections);
  CHECK_OB(import_mfun(env, fun))
  // paramters
  fun = new_DL_Func("void", "parameters",  (m_uint)parameters);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "input",  (m_uint)input);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "output",  (m_uint)output);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "neurons",  (m_uint)neurons);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "connections",  (m_uint)connections);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "type",  (m_uint)type);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "rate",  (m_uint)rate);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "layer",  (m_uint)layer);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int[]", "layers",  (m_uint)layers);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int[]", "bias",  (m_uint)bias);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("FANN_connect[]", "connection_array",  (m_uint)connection_array);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "weigth_array",  (m_uint)weigth_array);
    dl_func_add_arg(fun, "FANN_connect[]", "array");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float[]", "weigth",  (m_uint)weigth);
    dl_func_add_arg(fun, "int", "from");
    dl_func_add_arg(fun, "int", "to");
    dl_func_add_arg(fun, "float", "weigth");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float[]", "weigths",  (m_uint)get_weigths);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float[]", "weigths",  (m_uint)set_weigths);
    dl_func_add_arg(fun, "float[]", "f");
  CHECK_OB(import_mfun(env, fun))
// get/set user data
  fun = new_DL_Func("void", "disable_seed",  (m_uint)disable_seed);
  CHECK_OB(import_sfun(env, fun))
  fun = new_DL_Func("void", "enable_seed",  (m_uint)enable_seed);
  CHECK_OB(import_sfun(env, fun))

  fun = new_DL_Func("void", "load",  (m_uint)load);
  dl_func_add_arg(fun, "string", "file");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "save",  (m_uint)save);
    dl_func_add_arg(fun, "string", "file");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float[]", "run",  (m_uint)run);
    dl_func_add_arg(fun, "float[]", "in");
  CHECK_OB(import_mfun(env, fun))
  // FIXED POINT only
/*  fun = new_DL_Func("void", "decimal_point",  (m_uint)decimal_point);*/
/*  CHECK_OB(import_mfun(env, fun))*/
/*  fun = new_DL_Func("void", "multiplier",  (m_uint)multiplier);*/
/*  CHECK_OB(import_mfun(env, fun))*/

// training
  fun = new_DL_Func("void", "train",  (m_uint)train);
    dl_func_add_arg(fun, "float[]", "in");
    dl_func_add_arg(fun, "float[]", "out");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float[]", "test",  (m_uint)test);
    dl_func_add_arg(fun, "float[]", "in");
    dl_func_add_arg(fun, "float[]", "out");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "mse",  (m_uint)get_MSE);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "bit_fail",  (m_uint)get_bit_fail);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "reset_mse",  (m_uint)reset_MSE);
  CHECK_OB(import_mfun(env, fun))
  
// Training Data Training
  fun = new_DL_Func("void", "on_data",  (m_uint)train_on_data);
    dl_func_add_arg(fun, "FANN_data", "data");
    dl_func_add_arg(fun, "int",   "epoch");
    dl_func_add_arg(fun, "int",   "report");
    dl_func_add_arg(fun, "float", "error");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "on_file",  (m_uint)train_on_file);
    dl_func_add_arg(fun, "string", "filename");
    dl_func_add_arg(fun, "int",   "epoch");
    dl_func_add_arg(fun, "int",   "report");
    dl_func_add_arg(fun, "float", "error");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "epoch",  (m_uint)train_epoch);
    dl_func_add_arg(fun, "FANN_data", "data");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "epoch",  (m_uint)test_data);
    dl_func_add_arg(fun, "FANN_data", "data");
  CHECK_OB(import_mfun(env, fun))
/*  fun = new_DL_Func("void", "test",  (m_uint)train_test);*/
/*    dl_func_add_arg(fun, "FANN_data", "data");*/
/*  CHECK_OB(import_mfun(env, fun))*/

// Training Data Manipulation
  fun = new_DL_Func("void", "scale",  (m_uint)scale_train);
    dl_func_add_arg(fun, "FANN_data", "data");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "descale",  (m_uint)descale_train);
    dl_func_add_arg(fun, "FANN_data", "data");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "in_scaling_params",  (m_uint)input_scaling_params);
    dl_func_add_arg(fun, "FANN_data", "data");
    dl_func_add_arg(fun, "float", "min");
    dl_func_add_arg(fun, "float", "max");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "out_scaling_params",  (m_uint)output_scaling_params);
    dl_func_add_arg(fun, "FANN_data", "data");
    dl_func_add_arg(fun, "float", "min");
    dl_func_add_arg(fun, "float", "max");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "scaling_params",  (m_uint)scaling_params);
    dl_func_add_arg(fun, "FANN_data", "data");
    dl_func_add_arg(fun, "float", "imin");
    dl_func_add_arg(fun, "float", "imax");
    dl_func_add_arg(fun, "float", "omin");
    dl_func_add_arg(fun, "float", "omax");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "clear_scaling_params",  (m_uint)scaling_params);
  CHECK_OB(import_mfun(env, fun))
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "scale_input",  (m_uint)scale_input);
    dl_func_add_arg(fun, "float[]", "in");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "scale_output",  (m_uint)scale_output);
    dl_func_add_arg(fun, "float[]", "out");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "descale_input",  (m_uint)descale_input);
    dl_func_add_arg(fun, "float[]", "in");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "descale_output",  (m_uint)descale_output);
    dl_func_add_arg(fun, "float[]", "out");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "scale_in",  (m_uint)train_input_scale);
    dl_func_add_arg(fun, "FANN_data", "data");
    dl_func_add_arg(fun, "float", "min");
    dl_func_add_arg(fun, "float", "max");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "scale_out",  (m_uint)train_output_scale);
    dl_func_add_arg(fun, "FANN_data", "data");
    dl_func_add_arg(fun, "float", "min");
    dl_func_add_arg(fun, "float", "max");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("void", "scale",  (m_uint)train_scale);
    dl_func_add_arg(fun, "FANN_data", "data");
    dl_func_add_arg(fun, "float", "min");
    dl_func_add_arg(fun, "float", "max");
  CHECK_OB(import_mfun(env, fun))

  fun = new_DL_Func("int", "training_algorithm",  (m_uint)get_training_algorithm);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "training_algorithm",  (m_uint)set_training_algorithm);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "learning_rate",  (m_uint)get_learning_rate);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "learning_rate",  (m_uint)set_learning_rate);
    dl_func_add_arg(fun, "float", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "learning_momentum",  (m_uint)get_learning_momentum);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "learning_momentum",  (m_uint)set_learning_momentum);
    dl_func_add_arg(fun, "float", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "activation_function",  (m_uint)get_activation_function);
    dl_func_add_arg(fun, "int", "layer");
    dl_func_add_arg(fun, "int", "neuron");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "activation_function",  (m_uint)set_activation_function);
    dl_func_add_arg(fun, "int", "arg");
    dl_func_add_arg(fun, "int", "layer");
    dl_func_add_arg(fun, "int", "neuron");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "activation_function_layer",  (m_uint)set_activation_function_layer);
    dl_func_add_arg(fun, "int", "layer");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "activation_function_output",  (m_uint)set_activation_function_output);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "activation_steepness",  (m_uint)get_activation_steepness);
    dl_func_add_arg(fun, "int", "layer");
    dl_func_add_arg(fun, "int", "neuron");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "activation_steepness",  (m_uint)set_activation_steepness);
    dl_func_add_arg(fun, "int", "arg");
    dl_func_add_arg(fun, "int", "layer");
    dl_func_add_arg(fun, "int", "neuron");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "activation_steepness_layer",  (m_uint)set_activation_steepness_layer);
    dl_func_add_arg(fun, "int", "layer");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "activation_steepness_output",  (m_uint)set_activation_steepness_output);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "train_error_function",  (m_uint)get_train_error_function);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "train_error_function",  (m_uint)set_train_error_function);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "train_stop_function",  (m_uint)get_train_stop_function);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "train_stop_function",  (m_uint)set_train_stop_function);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "bit_fail_limit",  (m_uint)get_bit_fail_limit);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "bit_fail_limit",  (m_uint)set_bit_fail_limit);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "quickprop_decay",  (m_uint)get_quickprop_decay);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "quickprop_decay",  (m_uint)set_quickprop_decay);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "quickprop_mu",  (m_uint)get_quickprop_mu);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "quickprop_mu",  (m_uint)set_quickprop_mu);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "rprop_increase_factor",  (m_uint)get_rprop_increase_factor);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "rprop_increase_factor",  (m_uint)set_rprop_increase_factor);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "rprop_decrease_factor",  (m_uint)get_rprop_decrease_factor);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "rprop_decrease_factor",  (m_uint)set_rprop_decrease_factor);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "rprop_delta_min",  (m_uint)get_rprop_delta_min);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "rprop_delta_min",  (m_uint)set_rprop_delta_min);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "rprop_delta_max",  (m_uint)get_rprop_delta_max);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "rprop_delta_max",  (m_uint)set_rprop_delta_max);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "rprop_delta_zero",  (m_uint)get_rprop_delta_zero);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "rprop_delta_zero",  (m_uint)set_rprop_delta_zero);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "sarprop_weight_decay_shift",  (m_uint)get_sarprop_weight_decay_shift);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "sarprop_weight_decay_shift",  (m_uint)set_sarprop_weight_decay_shift);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "sarprop_step_error_threshold_factor",  (m_uint)get_sarprop_step_error_threshold_factor);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "sarprop_step_error_threshold_factor",  (m_uint)set_sarprop_step_error_threshold_factor);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
   fun = new_DL_Func("float", "sarprop_step_error_shift",  (m_uint)get_sarprop_step_error_shift);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "sarprop_step_error_shift",  (m_uint)set_sarprop_step_error_shift);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "sarprop_temperature",  (m_uint)get_sarprop_temperature);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "sarprop_temperature",  (m_uint)set_sarprop_temperature);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "cascade_output_change_fraction",  (m_uint)get_cascade_output_change_fraction);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "cascade_output_change_fraction",  (m_uint)set_cascade_output_change_fraction);
    dl_func_add_arg(fun, "float", "arg");
  CHECK_OB(import_mfun(env, fun))
   fun = new_DL_Func("int", "cascade_output_stagnation_epochs",  (m_uint)get_cascade_output_stagnation_epochs);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_output_stagnation_epochs",  (m_uint)set_cascade_output_stagnation_epochs);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "cascade_candidate_change_fraction",  (m_uint)get_cascade_candidate_change_fraction);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "cascade_candidate_change_fraction",  (m_uint)set_cascade_candidate_change_fraction);
    dl_func_add_arg(fun, "float", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_candidate_stagnation_epochs",  (m_uint)get_cascade_candidate_stagnation_epochs);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_candidate_stagnation_epochs",  (m_uint)set_cascade_candidate_stagnation_epochs);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "cascade_weight_multiplier",  (m_uint)get_cascade_weight_multiplier);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "cascade_weight_multiplier",  (m_uint)set_cascade_weight_multiplier);
    dl_func_add_arg(fun, "float", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "cascade_candidate_limit",  (m_uint)get_cascade_candidate_limit);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float", "cascade_candidate_limit",  (m_uint)set_cascade_candidate_limit);
    dl_func_add_arg(fun, "float", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_max_out_epochs",  (m_uint)get_cascade_max_out_epochs);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_max_out_epochs",  (m_uint)set_cascade_max_out_epochs);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_min_out_epochs",  (m_uint)get_cascade_min_out_epochs);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_min_out_epochs",  (m_uint)set_cascade_min_out_epochs);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
   fun = new_DL_Func("int", "cascade_max_cand_epochs",  (m_uint)get_cascade_max_cand_epochs);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_max_cand_epochs",  (m_uint)set_cascade_max_cand_epochs);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_min_cand_epochs",  (m_uint)get_cascade_min_cand_epochs);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_min_cand_epochs",  (m_uint)set_cascade_min_cand_epochs);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_activation_functions_count",  (m_uint)get_cascade_activation_functions_count);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int[]", "cascade_activation_functions",  (m_uint)get_cascade_activation_functions);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int[]", "cascade_activation_functions",  (m_uint)set_cascade_activation_functions);
    dl_func_add_arg(fun, "int[]", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_activation_steepnesses_count",  (m_uint)get_cascade_activation_steepnesses_count);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float[]", "cascade_activation_steepnesses",  (m_uint)get_cascade_activation_steepnesses);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("float[]", "cascade_activation_steepnesses",  (m_uint)set_cascade_activation_steepnesses);
    dl_func_add_arg(fun, "float[]", "arg");
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_num_candidate_groups",  (m_uint)get_cascade_num_candidate_groups);
  CHECK_OB(import_mfun(env, fun))
  fun = new_DL_Func("int", "cascade_num_candidate_groups",  (m_uint)set_cascade_num_candidate_groups);
    dl_func_add_arg(fun, "int", "arg");
  CHECK_OB(import_mfun(env, fun))

  CHECK_BB(import_class_end(env))
  return 1;
}
