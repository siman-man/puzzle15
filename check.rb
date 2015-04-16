total_count = 0
num = 0

File.open('result.txt') do |file|
  file.readlines.each do |line|
    line.chomp!

    count = line.split(':').last.to_i

    total_count += count
    num += 1
  end
end

puts "Average = %4.2f" % [total_count / num.to_f]
