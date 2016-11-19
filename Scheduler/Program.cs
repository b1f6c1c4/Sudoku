using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using Fclp;

namespace Scheduler
{
    internal class Program
    {
        private static string FindFile(string str, string path)
        {
            if (File.Exists(str))
                return str;

            var tmp = Path.Combine(path, str);
            if (File.Exists(tmp))
                return tmp;

            throw new FileNotFoundException();
        }

        private static void Main(string[] args)
        {
            var help = false;
            var gen = false;
            var number = 0;
            var table = 0;
            var floor = 0;
            var path = AppDomain.CurrentDomain.BaseDirectory;
            var generator = string.Empty;
            var problem = string.Empty;
            var assertor = string.Empty;
            var solver = string.Empty;

            {
                var p = new FluentCommandLineParser();

                p.SetupHelp("?", "help")
                 .Callback(
                           text =>
                           {
                               Console.WriteLine(text);
                               help = true;
                           });

                p.Setup<bool>('g', "generate")
                 .Callback(v => gen = v)
                 .SetDefault(false);

                p.Setup<int>('n', "number")
                 .Callback(v => number = v)
                 .SetDefault(10);

                p.Setup<int>('t', "table")
                 .Callback(v => table = v)
                 .SetDefault(60);

                p.Setup<int>('f', "floor")
                 .Callback(v => floor = v)
                 .SetDefault(25);

                p.Setup<string>("generator")
                 .Callback(s => generator = s)
                 .SetDefault("gen.exe");

                p.Setup<string>('b', "base-path")
                 .Callback(s => path = s);

                p.Setup<string>('p', "problem")
                 .Callback(s => problem = s)
                 .SetDefault("problem.txt");

                p.Setup<string>('a', "assertor")
                 .Callback(s => assertor = s)
                 .SetDefault("SudokuAssertor.exe");

                p.Setup<string>('s', "solver")
                 .Callback(s => solver = s)
                 .SetDefault("Sudoku.exe");

                p.Parse(args);
                if (help)
                    return;
            }

            if (gen)
                GenerateProblem(generator, number, table, floor, path);
            else
                problem = FindFile(problem, path);

            assertor = FindFile(assertor, path);
            solver = FindFile(solver, path);

            var ps = GetProblemList(problem);
            var tims = new List<int>();
            var tmos = 0;
            var errs = 0;
            File.Delete("problem.txt");
            for (var i = 0; i < ps.Count; i++)
            {
                Console.Write($"{i} / {ps.Count}");
                var p = ps[i];
                try
                {
                    var tim = Run(p, solver, assertor);
                    tims.Add(tim);
                }
                catch (TimeoutException)
                {
                    tmos++;
                }
                catch (ApplicationException e)
                {
                    errs++;
                    Console.Error.WriteLine(e.Message);
                }
                Console.Write("\r");
            }
            File.Delete("problem.txt");
            if (problem == "problem.txt")
                File.WriteAllLines(problem, ps);

            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.WriteLine("======== SUMMARY ========");
            Console.ForegroundColor = tmos == 0 ? ConsoleColor.Green : ConsoleColor.Red;
            Console.WriteLine($"Timeout : {(double)tmos / ps.Count:P2}");
            Console.ForegroundColor = errs == 0 ? ConsoleColor.Green : ConsoleColor.Red;
            Console.WriteLine($"Error   : {(double)errs / ps.Count:P2}");
            Console.ForegroundColor = tims.Count == ps.Count ? ConsoleColor.Green : ConsoleColor.Red;
            Console.WriteLine($"Accepted: {(double)tims.Count / ps.Count:P2}");
            Console.ForegroundColor = ConsoleColor.Cyan;
            if (tims.Any())
                Console.WriteLine($"Average : {tims.Average() / 1000.0:F3}ms");
            Console.ResetColor();
        }

        private static int Run(string problem, string solver, string assertor)
        {
            var proc =
                new Process
                    {
                        StartInfo =
                            new ProcessStartInfo(solver, "-t")
                                {
                                    CreateNoWindow = true,
                                    UseShellExecute = false,
                                    RedirectStandardInput = true,
                                    RedirectStandardOutput = true,
                                    RedirectStandardError = true,
                                    WorkingDirectory = Directory.GetCurrentDirectory()
                                }
                    };
            proc.Start();
            proc.StandardInput.Write(problem);
            proc.StandardInput.Close();
            if (!proc.WaitForExit(1000))
            {
                proc.Kill();
                throw new TimeoutException();
            }
            var res = proc.StandardOutput.ReadToEnd();
            var tim = proc.StandardError.ReadToEnd();
            proc.Close();

            File.WriteAllText("problem.txt", problem);
            var procA =
                new Process
                    {
                        StartInfo =
                            new ProcessStartInfo(assertor)
                                {
                                    CreateNoWindow = true,
                                    UseShellExecute = false,
                                    RedirectStandardInput = true,
                                    RedirectStandardError = true,
                                    WorkingDirectory = Directory.GetCurrentDirectory()
                                }
                    };
            procA.Start();
            procA.StandardInput.Write(res);
            procA.StandardInput.Close();
            procA.WaitForExit();
            var err = procA.StandardError.ReadToEnd();
            procA.Close();

            if (!string.IsNullOrWhiteSpace(err))
                throw new ApplicationException(err);

            return Convert.ToInt32(tim);
        }

        private static List<string> GetProblemList(string problem)
        {
            var lst = new List<string>();
            using (var sr = new StreamReader(problem))
            {
                var sb = new StringBuilder();
                while (true)
                {
                    sb.Clear();
                    for (var i = 0; i < 2 + 9 + 4; i++)
                    {
                        var str = sr.ReadLine();
                        if (str == null)
                            return lst;
                        sb.AppendLine(str);
                    }
                    lst.Add(sb.ToString());
                }
            }
        }

        private static void GenerateProblem(string generator, int number, int table, int floor, string path)
        {
            generator = FindFile(generator, path);

            var proc =
                new Process
                    {
                        StartInfo =
                            new ProcessStartInfo(generator)
                                {
                                    CreateNoWindow = true,
                                    UseShellExecute = false,
                                    RedirectStandardInput = true,
                                    WorkingDirectory = Directory.GetCurrentDirectory()
                                }
                    };
            proc.Start();

            var sw = proc.StandardInput;
            sw.WriteLine($"{number} {table} {floor} 0");
            sw.Close();
            proc.WaitForExit();
            proc.Close();
        }
    }
}
