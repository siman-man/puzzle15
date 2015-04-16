task :default do 
  system("g++ -O2 solver.cpp") 
end

task :run do
  system("./a.out < test_case3.txt")
end

task :solve do
  system("./a.out < test_case4.txt")
end

task :test do
  system("ruby generateBoard.rb > test_case3.txt")
end
