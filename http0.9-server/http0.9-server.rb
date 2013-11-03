#!/usr/bin/env ruby

# The stupidest possible web server you could conceive of. To start with we're
# only going to support HTTP version 0.9.
#
# To try it out:
#
#   1. Run this file, and the server will be listening for connections on port
#      3000
#   2. In your web browser of choice, navigate to localhost:3000/some-text-here
#   3. The browser will receive the HTML for a heading with "You asked for some-text-here"

require 'socket'

class HTTP09Server
  def initialize(port)
    Socket.tcp_server_loop(port.to_i) do |connection|
      # At this stage, we only support one-line HTTP 0.9 requests. They consist
      # of the word GET followed by a path. Example request:
      #
      #   GET /archive
      request = connection.readline.chomp

      # Ignore the request if it isn't a proper HTTP 0.9 request
      if match_data = request.match(%r{^GET (/[a-zA-Z0-9?&\-]*).*$})
        path = match_data.captures.first
        connection.write "<h1>You asked for #{path}</h1>"
      end

      connection.close
    end
  end
end

HTTP09Server.new(3000)
