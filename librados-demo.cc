#include <iostream>
#include <memory>
#include <rados/librados.hpp>
#include <string>

class RadosManager {
private:
  librados::IoCtx io_ctx;
  librados::Rados rados;
  std::string pool_name = "hello_world_pool";

  bool pool_created = false;
  bool rados_connected = false;

public:
  void InitRados(int argc, const char **argv) {
    int ret = 0;

    // Create a Rados object and initialize it
    {
      ret = rados.init("admin");
      if (ret < 0)
        throw std::runtime_error("couldn't initialize rados! error " +
                                 std::to_string(ret));
      std::cout << "we just set up a rados cluster object" << std::endl;
    }

    // Get config info of Rados object
    {
      ret = rados.conf_parse_argv(argc, argv);
      if (ret < 0)
        // This really can't happen, but we need to check to be a good citizen.
        throw std::runtime_error("failed to parse config options! error " +
                                 std::to_string(ret));
      std::cout << "we just parsed our config options" << std::endl;

      // Apply the config file if the user specified
      for (int i = 0; i < argc; ++i) {
        if ((strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "--conf") == 0)) {
          ret = rados.conf_read_file(argv[i + 1]);
          if (ret < 0)
            // This could fail if the config file is malformed, but it'd be
            // hard.
            throw std::runtime_error("failed to parse config file " +
                                     std::string(argv[i + 1]) + "! error" +
                                     std::to_string(ret));
          break;
        }
      }
    }

    // Connect to the cluster
    {
      ret = rados.connect();
      if (ret < 0)
        throw std::runtime_error("couldn't connect to cluster! error " +
                                 std::to_string(ret));
      rados_connected = true;
      std::cout << "we just connected to the rados cluster" << std::endl;
    }

    // Create a instead of scribbling over real data.
    {
      ret = rados.pool_create(pool_name.c_str());
      if (ret < 0)
        throw std::runtime_error("couldn't create pool! error " +
                                 std::to_string(ret));
      pool_created = true;
      std::cout << "we just created a new pool named " << pool_name
                << std::endl;
    }

    // Create an "IoCtx" which is used to do IO to a pool
    {
      ret = rados.ioctx_create(pool_name.c_str(), io_ctx);
      if (ret < 0) {
        throw std::runtime_error("couldn't set up ioctx! error " +
                                 std::to_string(ret));
      }
      std::cout << "we just created an ioctx for our pool" << std::endl;
    }
  }

  // Put an object with content
  void Put(const std::string &object_name, const std::string &content) {
    int ret = 0;
    librados::bufferlist bl;
    bl.append(content);

    // Send it to an object.
    ret = io_ctx.write_full(object_name, bl);
    if (ret < 0) {
      throw std::runtime_error("couldn't write object! error " +
                               std::to_string(ret));
    }
    std::cout << "we just wrote new object " << object_name
              << ", with contents\n"
              << content << std::endl;
  }

  // Read an object
  std::string GetBalanced(const std::string &object_name) {
    int ret = 0;
    librados::bufferlist read_buf;
    librados::ObjectReadOperation read_op;

    ret = io_ctx.operate(object_name, &read_op, &read_buf,
                         librados::OPERATION_BALANCE_READS);

    if (ret < 0)
      throw std::runtime_error("couldn't read object! error " +
                               std::to_string(ret));
    std::cout << "we read our object " << object_name << ", and got back "
              << ret << " bytes with contents\n";
    std::string read_string;
    read_buf.begin().copy(ret, read_string);
    std::cout << read_string << std::endl;
    return read_string;
  }

  // Remove our pool and then shut down the connection gracefully.
  ~RadosManager() {
    if (pool_created) {
      int delete_ret = rados.pool_delete(pool_name.c_str());
      if (delete_ret < 0) {
        std::cerr << "We failed to delete our test pool!" << std::endl;
        exit(EXIT_FAILURE);
      }
    }

    if (rados_connected)
      rados.shutdown();
  }
};

int main(int argc, const char **argv) {
  std::string hello = "hello world";
  std::string object_name = "hello_object";

  try {
    std::unique_ptr<RadosManager> rm = std::make_unique<RadosManager>();
    rm->InitRados(argc, argv);
    rm->Put(object_name, hello);
    rm->GetBalanced(object_name);
  } catch (const std::runtime_error &e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}