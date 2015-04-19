task :default do 
  system("g++ -O2 -o solver solver.cpp") 
end

task :run do
  system("g++ -O2 -o solver solver.cpp") 
  system("./solver < test_case3.txt")
end

task :solve do
  system("./solver < test_case4.txt")
end

task :check do
  system("g++ -O2 -o solver solver.cpp") 
  system("./solver < test_case1.txt > result.txt")
  system("./solver < test_case2.txt >> result.txt")
  system("./solver < test_case3.txt >> result.txt")
  system("./solver < test_case4.txt >> result.txt")
  system("./solver < test_case5.txt >> result.txt")
  system("./solver < test_case6.txt >> result.txt")
  system("./solver < test_case7.txt >> result.txt")
  system("./solver < test_case8.txt >> result.txt")
  system("./solver < test_case9.txt >> result.txt")
  system("./solver < test_case10.txt >> result.txt")
  system("./solver < test_case11.txt >> result.txt")
  system("./solver < test_case12.txt >> result.txt")
  system("./solver < test_case13.txt >> result.txt")
  system("./solver < test_case14.txt >> result.txt")
  system("./solver < test_case15.txt >> result.txt")
  system("cat result.txt")
  system("ruby check.rb")
end

task :test do
  system("ruby generateBoard.rb > test_case3.txt")
end
