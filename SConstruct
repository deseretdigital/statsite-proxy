import platform

optimize = '-O3'

envmurmur = Environment(CPPPATH = ['deps/murmurhash/'], CPPFLAGS="-fno-exceptions " + optimize)
murmur = envmurmur.Library('murmur', Glob("deps/murmurhash/*.cpp"))

envinih = Environment(CPATH = ['deps/inih/'], CFLAGS=optimize)
inih = envinih.Library('inih', Glob("deps/inih/*.c"))

envketama = Environment(CPATH = ['deps/libketama/'], CFLAGS=optimize)
ketama = envketama.Library('ketama', Glob("deps/libketama/md5.c") + Glob("deps/libketama/ketama.c")) 

env_statsite_with_err = Environment(CCFLAGS = '-std=c99 -D_GNU_SOURCE -Wall -Werror ' + optimize + ' -pthread -Ideps/inih/ -Ideps/libev/ -Ideps/libketama/ -Isrc/')
env_statsite_without_err = Environment(CCFLAGS = '-std=c99 -D_GNU_SOURCE ' + optimize + ' -pthread -Ideps/inih/ -Ideps/libev/ -Ideps/libketama/ -Isrc/')

objs = env_statsite_with_err.Object('src/proxy', 'src/proxy.c')               + \
        env_statsite_with_err.Object('src/hashring', 'src/hashring.c')        + \
        env_statsite_with_err.Object('src/hashmap', 'src/hashmap.c')          + \
        env_statsite_with_err.Object('src/heap', 'src/heap.c')                + \
        env_statsite_with_err.Object('src/config', 'src/config.c')            + \
        env_statsite_without_err.Object('src/networking', 'src/networking.c') + \
        env_statsite_without_err.Object('src/conn_handler', 'src/conn_handler.c')

statsite_proxy_libs = ["m", "pthread", murmur, inih, ketama]
if platform.system() == 'Linux':
   statsite_libs.append("rt")

statsite_proxy = env_statsite_with_err.Program('statsite_proxy', objs + ["src/statsite_proxy.c"], LIBS=statsite_proxy_libs)
statsite_proxy_test = env_statsite_without_err.Program('test_runner', objs + Glob("tests/runner.c"), LIBS=statsite_proxy_libs + ["check"])

# By default, only compile statsite_proxy
Default(statsite_proxy)
