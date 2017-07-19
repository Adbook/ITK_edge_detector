#include "mainwindow.hxx"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_input_set(true), m_output_set(false)
{
	ui->setupUi(this);
	
	ui->button_output->setEnabled(false);
	ui->button_save->setEnabled(false);
	ui->sb_upperthresh->setEnabled(false);
	ui->sb_lowerthresh->setEnabled(false);

	ui->input_label->setText("No input picture chosen");
	//buttons
	connect(ui->button_input, SIGNAL(clicked(bool)), this, SLOT(choose_input_file()));
	connect(ui->button_output, SIGNAL(clicked(bool)), this, SLOT(choose_output_file()));
	connect(ui->button_save, SIGNAL(clicked(bool)), this, SLOT(save_output()));
	//spinboxes (number inputs)
	connect(ui->sb_variance, SIGNAL(valueChanged(double)), this, SLOT(update_variance()));
	connect(ui->sb_upperthresh, SIGNAL(valueChanged(double)), this, SLOT(update_upper_thresh()));
	connect(ui->sb_lowerthresh, SIGNAL(valueChanged(double)), this, SLOT(update_lower_thresh()));
	//checkbox
	connect(ui->checkbox_autothreshold, SIGNAL(stateChanged(int)), this, SLOT(toggle_auto_threshold()));


	//itk Object creation
	m_reader = ImageReaderType::New();
	m_writer = ImageWriterType::New();
	m_cast_filter = CastFilterType::New();
	
	m_filter = FilterType::New();
	m_filter->SetVariance(ui->sb_variance->value());
	m_filter->SetAutomaticThresholdingOn();

	m_filter->SetInput(m_reader->GetOutput());
	m_cast_filter->SetInput(m_filter->GetOutput());
	m_writer->SetInput(m_cast_filter->GetOutput());

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
    m_reader->SetFileName(filename.toStdString());
    ui->button_output->setEnabled(true);
    m_input_set = true;
}

void MainWindow::choose_output_file()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
                           "~/untitled.png");
	std::cout << filename.toStdString() << std::endl;
	m_writer->SetFileName(filename.toStdString());
	ui->button_save->setEnabled(true);
	m_output_set = true;
}

void MainWindow::save_output()
{
	if(!m_output_set) return;
	try {
	        m_writer->Update();
	    }catch(itk::ExceptionObject & err)
	    {
	        std::cerr << "Exception Caught while writing\n";
	        std::cerr << err << std::endl;
	    }
}

void MainWindow::update_variance()
{
	std::cout << "variance "  << ui->sb_variance->value() << std::endl;
	m_filter->SetVariance(ui->sb_variance->value());
	save_output();
}

void MainWindow::toggle_auto_threshold()
{
	std::cout << "checkbox " << ui->checkbox_autothreshold->isChecked() << std::endl;
	if(ui->checkbox_autothreshold->isChecked()){
		ui->sb_upperthresh->setEnabled(false);
		ui->sb_lowerthresh->setEnabled(false);
		m_filter->SetAutomaticThresholdingOn();
	}else{
		ui->sb_upperthresh->setEnabled(true);
		ui->sb_lowerthresh->setEnabled(true);
		m_filter->SetAutomaticThresholdingOff();
	}
	save_output();
}

void MainWindow::update_upper_thresh()
{
	std::cout << "upper thresh " << ui->sb_upperthresh->value() << std::endl;
	m_filter->SetUpperThreshold(ui->sb_upperthresh->value());
	save_output();
}

void MainWindow::update_lower_thresh()
{
	std::cout << "lower thresh " << ui->sb_lowerthresh->value() << std::endl;
	m_filter->SetLowerThreshold(ui->sb_lowerthresh->value());
	save_output();
}