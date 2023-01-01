require 'csv'


# Accel: linear triangle:
#   0 -> .5 C/s^2 in 400ms
#   .5 -> 0 C/s^2 in 400ms

class System
  def initialize
    @t = 0
    @on = true
    @accel_start_at = 0
    @pos = 25
    @velocity = 0
    @accel = 0
    @log = []
  end

  def step!(t, new_on)
    if new_on != on
      @on = new_on
      @accel_start_at = t
    end
    if (t - @accel_start_at) < 400
      @accel = 0.5 * (t - @accel_start_at) / 400.0
    elsif (t - @accel_start_at) < 800
      @accel = 0.5 - (0.5 * (t - @accel_start_at) / 400.0)
    else
      @accel = 0
    end
    @accel = -1 * @accel unless @on

    @velocity = @velocity + (@accel * (t - @t))
    @pos = @pos + (@velocity * (t - @t))
    @t = t
    @log.push([@t, @pos, @velocity, @accel])
  end
end


sys = System.new
res = []
(0..60_000).step(100).each do |t|
  sys.step!(t, true)
end
(60_000..200_000).step(100).each do |t|
  sys.step(t, (t % 60) > 30)
end
(200_000..240_000).step(100).each do |t|
  sys.step!(t, true)
end
(240_000..400_000).step(100).each do |t|
  sys.step!(t, false)
end


