from tkinter import *
from tkinter import messagebox, simpledialog
from tkinter.ttk import *
import subprocess

class Application(Frame):
    def __init__(self, master=None):
        super().__init__(master)  # super()代表父类的定义 不是父类对象
        self.master = master
        self.pack()
        self.createWidget()

    def createWidget(self):
        self.label1 = Label(self, text="DBMS")
        self.label1.grid(row=0, column=1)
        # 创建用户名输入框
        self.label2 = Label(self, text="用户名")
        self.label2.grid(row=1, column=0)

        v1 = StringVar()
        self.entry1 = Entry(self, textvariable=v1)
        self.entry1.grid(row=1, column=1)


        # 创建密码输入框
        self.label3 = Label(self, text="密码")
        self.label3.grid(row=2, column=0)

        v2 = StringVar()
        self.entry2 = Entry(self, textvariable=v2)
        self.entry2.grid(row=2, column=1)

        #  创建登录按钮
        self.btn1 = Button(self,text="登录", command=self.login)
        self.btn1.grid(row=5, column=1)

        # 创建一个退出按钮
        self.btn_Quit = Button(self, text="退出", command=root.destroy)
        self.btn_Quit.grid(row=5, column=2)

    def login(self):
        username = self.entry1.get()
        password = self.entry2.get()

        if username == "" and password == "":
            create_frame1()

        else:
            messagebox.showinfo("sorry", "登录失败")


def create_frame1(p=None):
    global root1
    root.destroy()
    root1 = Tk()
    root1.geometry("1000x700+250+50")
    root1.title("DBMS")
    Button(root1, text="Run SQL", command=frame1_button1).pack(side="left", padx="10",anchor="n")
    Button(root1, text="退回登录界面", command=return_to_main_frame).pack(side="left", padx="10",anchor="n")
    # sql = simpledialog.askstring("查找", "请输入SQL语句")
    root1.mainloop()


def frame1_button1():
    global root1
    sql = simpledialog.askstring("查找", "请输入SQL语句", parent = root1)
    # 调用main.exe 以sql作为参数
    result = ""
    order = rf"python3 main.exe {sql}"
    pi= subprocess.Popen(order,shell=True,stdout=subprocess.PIPE)
    for i in iter(pi.stdout.readline,'b'):
        result += i.decode('gbk')
    
    # 得到返回值，然后输出
    messagebox.showinfo("Result", result,parent=root1)

def return_to_main_frame():
    root1.destroy()
    
    main_frame()

def main_frame():
    global root
    # 主窗口对象 root
    root = Tk()
    # 通过geometry方法确定窗口在屏幕中的位置以及窗口的大小
    root.geometry("500x200+500+100")
    root.title("DBMS")

    app = Application(master=root)
    # 进入事件主循环 等待响应
    root.mainloop()


if __name__ == "__main__":
    main_frame()



