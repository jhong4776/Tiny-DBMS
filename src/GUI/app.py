import sys
import tkinter as tk


#--------------------查询函数---------------------------
def sql_connect():
    listbox_show.delete(0,'end') #初始化清空显示区
    m = listbox_name.curselection()[0]
    print('m=',m)
    list_name = listbox_name.get(m)[0]
    varlue = entry_varlue.get()
    print('list_name=',list_name)
    print('var=',varlue)

    while list_name == 'Student_ID':
        sql_sr = "select * from Student where student.student_id = " + "'" + varlue +"'"
        print(sql_sr)
        break
    while list_name == 'Name':
        sql_sr = "select * from Student where student.name = " + "'" + varlue +"'"
        print(sql_sr)
        break

    while list_name == 'Age':
        sql_sr = "select * from Student where student.age = " + "'" + varlue +"'"
        print(sql_sr)
        break
    while list_name == 'Sex':
        sql_sr = "select * from Student where student.sex = " + "'" + varlue +"'"
        print(sql_sr)
        break
    while list_name == 'Class':
        sql_sr = "select * from Student where student.class = " + "'" + varlue +"'"
        print(sql_sr)
        break

    mydb = open("main.exe")
    myresult = mydb.run(sql_sr)
    for x in myresult:
        print (x)
        listbox_show.insert('end',x)
#--------------------------------------------------------


#------------界面初始化时加载选项列表--------------------
sql_n = "SELECT * FROM Table"

mydb = open("main.exe")
myresult = mydb.run(sql_n)

#--------------------------------------------------------

#----------窗体部分--------------------------------------
entry_list_name = tk.Entry()
entry_list_name.pack()

listbox_name = tk.Listbox()
listbox_name.pack()
for i in myresult:
    listbox_name.insert('end',i) #加载选项列表

entry_varlue = tk.Entry()
entry_varlue.pack()

button_select = tk.Button(text = '查找',command = sql_connect)
button_select.pack()

listbox_show = tk.Listbox()
listbox_show.pack()


root.mainloop()
#--------------------------------------------------------