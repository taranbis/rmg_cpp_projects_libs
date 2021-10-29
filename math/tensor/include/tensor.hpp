#ifndef _TENSOR_HEADER_HPP
#define _TENSOR_HEADER_HPP 1
#pragma once

//TODO: create class Shape
struct Shape {
 public:
  Shape()
    : ndim_(0),
    num_heap_allocated_(0),
    data_heap_(nullptr) {}

  /*!
  * \brief constructor from a vector of index_t
  * \param v the vector
  */
  explicit Shape(const std::vector<index_t> &v)
    : ndim_(v.size()) {
    if (ndim_ <= kStackCache) {
      data_heap_ = nullptr;
      num_heap_allocated_ = 0;
      std::copy(v.begin(), v.end(), data_stack_);
    } else {
      data_heap_ = new index_t[ndim_];
      num_heap_allocated_ = ndim_;
      std::copy(v.begin(), v.end(), data_heap_);
    }
  }
  /*!
  * \brief constructor one dimmension shape
  * \param s1 size of the first dimmension
  */
  explicit Shape(index_t s1)
    : ndim_(1) {
    if (ndim_ <= kStackCache) {
      data_heap_ = nullptr;
      num_heap_allocated_ = 0;
      data_stack_[0] = s1;
    } else {
      data_heap_ = new index_t[ndim_];
      num_heap_allocated_ = ndim_;
      data_heap_[0] = s1;
    }
  }
  /*!
  * \brief constructor two dimmension shape
  * \param s1 size of the first dimmension
  * \param s2 size of the second dimmension
  */
  Shape(index_t s1, index_t s2)
    : ndim_(2) {


  /*!
  * \brief constructor five dimmension shape
  * \param s1 size of the first dimmension
  * \param s2 size of the second dimmension
  * \param s3 size of the third dimmension
  * \param s4 size of the fourth dimmension
  * \param s5 size of the fifth dimmension
  */
  Shape(index_t s1, index_t s2, index_t s3, index_t s4, index_t s5)
    : ndim_(5) {
    if (ndim_ <= kStackCache) {
      data_heap_ = nullptr;
      num_heap_allocated_ = 0;
      data_stack_[0] = s1;
      data_stack_[1] = s2;
      data_stack_[2] = s3;
      data_stack_[3] = s4;
      data_stack_[4] = s5;
    } else {
      data_heap_ = new index_t[ndim_];
      num_heap_allocated_ = ndim_;
      data_heap_[0] = s1;
      data_heap_[1] = s2;
      data_heap_[2] = s3;
      data_heap_[3] = s4;
      data_heap_[4] = s5;
    }
  }


enum DeviceType {
  kCPU = 1,
  kGPU = 2,
  kCPUPinned = 3
};
class Context {
 public:
  /*!
  * \brief Context constructor
  * \param type type of the device
  * \param id id of the device
  */
  Context(const DeviceType &type, int id) : type_(type), id_(id) {}

  DeviceType GetDeviceType() const { return type_; }
  int GetDeviceId() const { return id_; }

  static Context gpu(int device_id = 0) {
    return Context(DeviceType::kGPU, device_id);
  }

  /*!
   * \brief Return a CPU context
   * \param device_id id of the device. this is not needed by CPU
   * \return the corresponding CPU context
   */
  static Context cpu(int device_id = 0) {
    return Context(DeviceType::kCPU, device_id);
  }

 private:
  DeviceType type_;
  int id_;
};

template <typename DataType>
class Tensor
{
public:
    explicit Tensor(const TensorHandle &handle);

    Tensor(const std::vector<uint32_t> &shape, const Context &context);

    Tensor(const Shape &shape, const Context &context);

    Tensor(const DataType *data, std::size_t size);

    Tensor(const DataType *data, const Shape &shape, const Context &context);

    Tensor(const std::vector<DataType> &data, const Shape &shape, const Context &context);

    explicit Tensor(const std::vector<DataType> &data);

    // operator+;
    // operator-;
    // operator*;
    // operator/;
    // operator%;
    // operator+;
    // operator-;
    // operator*;
    // operator/;
    // operator%;

    Tensor ArgmaxChannel();

  /*!
  * \brief Do a synchronize copy from a contiguous CPU memory region.
  *
  *  This function will call WaitToWrite before the copy is performed.
  *  This is useful to copy data from existing memory region that are
  *  not wrapped by NDArray(thus dependency not being tracked).
  *
  * \param data the data source to copy from.
  * \param size the memory size we want to copy from.
  */
    // void SyncCopyFromCPU(const DataType *data, size_t size);


	
    //   void SyncCopyFromCPU(const std::vector<DataType> &data);
	// int GetDType() const;
	// const DataType *GetData() const;
	// TensorHandle GetHandle() const { return blob_ptr_->handle_; }


	  /*!
	* \brief save a list of NDArrays to binary file.
	* \param file_name name of the binary file.
	* \param array_list a list of NDArrays.
	*/
	// static void Save(const std::string &file_name,
	// 				const std::vector<NDArray> &array_list);

	  static std::vector<NDArray> LoadFromBufferToList(const void *buffer, size_t size);		
	static std::map<std::string, NDArray> LoadFromBufferToMap(const void *buffer, size_t size);


  /*!
  * \brief Load NDArrays from buffer.
  * \param buffer Pointer to buffer. (ie contents of param file)
  * \param size Size of buffer
  * \param array_list a list of NDArrays returned, do not fill the list if
  * nullptr is given.
  * \param array_map a map from names to NDArrays returned, do not fill the map
  * if nullptr is given or no names is stored in binary file.
  */
  static void LoadFromBuffer(const void *buffer, size_t size,
                   std::vector<NDArray> *array_list = nullptr,
                   std::map<std::string, NDArray> *array_map = nullptr);

  /*!
  * \brief Load NDArrays from binary file.
  * \param file_name name of the binary file.
  * \param array_list a list of NDArrays returned, do not fill the list if
  * nullptr is given.
  * \param array_map a map from names to NDArrays returned, do not fill the map
  * if nullptr is given or no names is stored in binary file.
  */
  static void Load(const std::string &file_name,
                   std::vector<NDArray> *array_list = nullptr,
                   std::map<std::string, NDArray> *array_map = nullptr);
  /*!
  * \brief Load map of NDArrays from binary file.
  * \param file_name name of the binary file.
  * \return a list of NDArrays.
  */
  static std::map<std::string, NDArray> LoadToMap(const std::string &file_name);
  /*!
  * \brief Load list of NDArrays from binary file.
  * \param file_name name of the binary file.
  * \return a map from names to NDArrays.
  */
  static std::vector<NDArray> LoadToList(const std::string &file_name);



    void WaitToRead() const;
  /*!
  * \brief Block until all the pending read/write operations with respect
  *    to current NDArray are finished, and write can be performed.
  */
  void WaitToWrite();
  /*!
  * \brief Block until all the pending read/write operations with respect
  *    to current NDArray are finished, and read/write can be performed.
  */
  static void WaitAll();


};

#endif
