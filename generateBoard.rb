class Board
  attr_accessor :board

  DY = [-1,0,1,0]
  DX = [0,1,0,-1]

  def initialize
    @board = Array.new(4){ Array.new(4) }
    y = 0
    x = 0

    1.upto(16) do |num|
      y = (num-1) / 4
      x = (num-1) % 4

      board[y][x] = (num != 16)? num : '*'
    end

    100000.times do
      d = rand(4)

      dy = y + DY[d]
      dx = x + DX[d]

      if !wall?(dy, dx)
        swap(y,x,dy,dx)
        y, x = dy, dx
      end
    end
  end

  def swap(y1, x1, y2, x2)
    board[y1][x1], board[y2][x2] = board[y2][x2], board[y1][x1]
  end

  def generate
    4.times do |i|
      puts board[i].join(' ')
    end
  end

  def wall?(y, x)
    y < 0 || y >= 4 || x < 0 || x >= 4
  end
end

board = Board.new
board.generate
