require 'http/2'
require 'socket'

socket = TCPSocket.new('twitter.com', 80)

conn = HTTP2::Client.new
conn.on(:frame) {|data| socket << data }

while bytes = socket.read
  conn << bytes
end
