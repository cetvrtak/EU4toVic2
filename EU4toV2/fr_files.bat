@echo off
robocopy "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\french_revolution\history\provinces" "D:\Games\Victoria II\mod\French Revolution\history\provinces" /e /mt /z /xf "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\french_revolution\history\provinces\germany\608 - Neuchatel.txt"
robocopy "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\french_revolution\history\wars" "D:\Games\Victoria II\mod\French Revolution\history\wars" /e /mt /z
robocopy "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\FR Files" "D:\Games\Victoria II\mod\French Revolution" /e /mt /z /xf "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\FR Files\countries.txt"
robocopy "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\french_revolution\technologies" "D:\Games\Victoria II\mod\French Revolution\technologies" /e /mt /z
robocopy "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\french_revolution\inventions" "D:\Games\Victoria II\mod\French Revolution\inventions" /e /mt /z
copy "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\french_revolution\localisation\00_FR_technology.csv" "D:\Games\Victoria II\mod\French Revolution\localisation\00_FR_technology.csv"
robocopy "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\french_revolution\gfx\pictures\tech" "D:\Games\Victoria II\mod\French Revolution\gfx\pictures\tech" /e /mt /z
robocopy "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\french_revolution\units" "D:\Games\Victoria II\mod\French Revolution\units" /e /mt /z

robocopy "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\french_revolution\common\countries" "D:\Games\Victoria II\mod\French Revolution\common\countries" /e /mt /z
robocopy "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\french_revolution\history\countries" "D:\Games\Victoria II\mod\French Revolution\history\countries" /e /mt /z
robocopy "D:\source\repos\eu4tovic2\Release\EU4ToVic2\output\french_revolution\history\units" "D:\Games\Victoria II\mod\French Revolution\history\units" /e /mt /z
