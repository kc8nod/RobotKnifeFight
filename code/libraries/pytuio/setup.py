from distutils.core import setup
import tuio

setup(name='pytuio',
      version=tuio.__version__,
      description=tuio.__doc__,
      author=tuio.__author__,
      author_email='jannis@leidel.info',
      license=tuio.__license__,
      url=tuio.__url__,
      packages=['tuio',],
)
