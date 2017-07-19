#include "mainwindow.hxx"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	
	//buttons
	connect(ui->button_input, SIGNAL(clicked(bool)), this, SLOT(choose_input_file()));
	connect(ui->button_output, SIGNAL(clicked(bool)), this, SLOT(choose_output_file()));

	//spinboxes (number inputs)
	connect(ui->sb_variance, SIGNAL(valueChanged(double)), this, SLOT(update_variance()));
	connect(ui->sb_upperthresh, SIGNAL(valueChanged(double)), this, SLOT(update_upper_thresh()));
	connect(ui->sb_lowerthresh, SIGNAL(valueChanged(double)), this, SLOT(update_lower_thresh()));

	//checkbox
	connect(ui->checkbox_autothreshold, SIGNAL(stateChanged(int)), this, SLOT(toggle_auto_threshold()));

	
}


MainWindow::~MainWindow(){

	
}

void MainWindow::choose_input_file()
{
	QString filename = QFileDialog::getOpenFileName(this,
    tr("Open Image"), "~", tr("Image Files (*.png *.jpg *.bmp)"));
    std::cout << filename.toStdString() << std::endl;

    QPixmap pic (filename);
    ui->input_label->setPixmap(pic);
}

void MainWindow::choose_output_file()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
                           "~/untitled.png");
	std::cout << filename.toStdString() << std::endl;
}

void MainWindow::update_variance()
{
	std::cout << "variance "  << ui->sb_variance->value() << std::endl;
}

void MainWindow::toggle_auto_threshold()
{
	std::cout << "checkbox " << ui->checkbox_autothreshold->isChecked() << std::endl;
}

void MainWindow::update_upper_thresh()
{
	std::cout << "upper thresh " << ui->sb_upperthresh->value() << std::endl;
}

void MainWindow::update_lower_thresh()
{
	std::cout << "lower thresh " << ui->sb_lowerthresh->value() << std::endl;
}