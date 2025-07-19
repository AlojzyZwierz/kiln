import datetime
import os

def get_build_time():
    return datetime.datetime.now().strftime("%y%m%d%H")

def inject_build_time(env):
    build_time = get_build_time()
    env.Append(CPPDEFINES=[("BUILD_TIME", '"{}"'.format(build_time))])

def before_build(source, target, env):
    inject_build_time(env)

# Rejestracja hooka
Import("env")
before_build(None, None, env)
