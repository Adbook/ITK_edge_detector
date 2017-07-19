#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <iostream>
//#include 
namespace Ui{
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget * parent = NULL);
	~MainWindow();

protected slots:
	void choose_input_file();
	void choose_output_file();
	void update_variance();
	void toggle_auto_threshold();
	void update_upper_thresh();
	void update_lower_thresh();
private:
	Ui::MainWindow *ui;


};



#endif