import urllib2
import time

for i in xrange(5):
    urllib2.urlopen('http://yunrrh.local/arduino/digital/13/0')
    time.sleep(1)
    urllib2.urlopen('http://yunrrh.local/arduino/digital/13/1')