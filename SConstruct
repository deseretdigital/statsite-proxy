import platform

envmurmur = Environment(CPPPATH = ['deps/murmurhash/'], CPPFLAGS="-fno-exceptions -O3")
murmur = envmurmur.Library('murmur', Glob("deps/murmurhash/*.cpp"))

envinih = Environment(CPATH = ['deps/inih/'], CFLAGS="-O3")
inih = envinih.Library('inih', Glob("deps/inih/*.c"))

envketama = Environment(CPATH = ['deps/libketama/'], CFLAGS="-O3")
ketama = envketama.Library('ketama', Glob("deps/libketama/*.c")) 

env_statsite_with_err = Environment(CCFLAGS = '-std=c99 -D_GNU_SOURCE -Wall -Werror -O3 -pthread -Ideps/inih/ -Ideps/libev/ -Ideps/libketama/ -Isrc/')
env_statsite_without_err = Environment(CCFLAGS = '-std=c99 -D_GNU_SOURCE -O3 -pthread -Ideps/inih/ -Ideps/libev/ -Ideps/libketama/ -Isrc/')

objs = env_statsite_with_err.Object('src/hashmap', 'src/hashmap.c')           + \
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
