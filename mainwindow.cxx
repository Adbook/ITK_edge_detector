#include "mainwindow.hxx"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_input_set(false), m_output_set(false), m_timer_timed_out(true)
{
	ui->setupUi(this);
	ui->widget->resize(256, 256);
	


	ui->button_output->setEnabled(false);
	ui->button_save->setEnabled(false);
	ui->sb_upperthresh->setEnabled(false);
	ui->sb_lowerthresh->setEnabled(false);

	ui->input_label->setText("Please choose an input picture");

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
	m_itk_to_vtk_filter = ITKToVTKFilterType::New();
	
	//edge detection filter configuration
	m_filter->SetVariance(ui->sb_variance->value());
	m_filter->SetAutomaticThresholdingOn();
	
	//process configuration
	m_filter->SetInput(m_reader->GetOutput());
	m_cast_filter->SetInput(m_filter->GetOutput());
	m_itk_to_vtk_filter->SetInput(m_filter->GetOutput());
	m_writer->SetInput(m_cast_filter->GetOutput());
	
	//vtk image viewer creation and configuration
	vtkImageViewer *viewer = vtkImageViewer::New();

	imageFlip = vtkImageFlip::New();
	imageFlip->SetFilteredAxis(1);
	imageFlip->SetInputData(m_itk_to_vtk_filter->GetOutput());
	viewer->SetInputData(imageFlip->GetOutput());	

	viewer->SetColorLevel(128);
	viewer->SetColorWindow(256);
	
	//vtk render window creation and configuration
	vtkRenderWindow *renderWindow = viewer->GetRenderWindow();
	
	ui->widget->SetRenderWindow(renderWindow);
	renderWindow->Render();

	//QTimer configuration, timeout used to limit writes
	m_timer = new QTimer();
	connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
	m_timer->start(500);

	if(!m_input_set) ui->widget->setEnabled(false);
	this->resize(minimumSizeHint());

}


MainWindow::~MainWindow(){

	
}

void MainWindow::timeout()
{
	if(!m_timer_timed_out){
		m_timer_timed_out = true;
		updateVTKWidget();
	}

}
void MainWindow::choose_input_file()
{
	QString filename = QFileDialog::getOpenFileName(this,
    tr("Open Image"), "../data/", tr("Image Files (*.png *.jpg *.bmp)"));

    QPixmap pic (filename);
    ui->input_label->setPixmap(pic);
    m_reader->SetFileName(filename.toStdString());

    
    if(!m_input_set){
    	ui->button_output->setEnabled(true);
    	ui->widget->setEnabled(true);
    	m_input_set = true;
	}

	//update necessary to get the size of the input image. 
	m_reader->UpdateLargestPossibleRegion();
    m_reader->Update();


    //vtk widget resizing
    InputImageType::SizeType size = m_reader->GetOutput()->GetLargestPossibleRegion().GetSize();

    //display
    updateVTKWidget();

    ui->widget->setFixedSize(pic.size());
    //window resizing to smallest possible size
    this->resize(minimumSizeHint());

}

void MainWindow::choose_output_file()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
                           "~/output.png");
	m_writer->SetFileName(filename.toStdString());
	ui->button_save->setEnabled(true);
	m_output_set = true;

	m_itk_to_vtk_filter->Update();
	ui->widget->GetRenderWindow()->Render();
}

void MainWindow::updateVTKWidget()
{
	//timer used to limit rate of writes/filter updates
	if(!m_timer_timed_out) return;
	m_timer_timed_out = false;

	try {
			
			m_itk_to_vtk_filter->Update();
			imageFlip->Update();
	}catch(itk::ExceptionObject & err)
	{
	    std::cerr << "Exception Caught while processing\n";
	    std::cerr << err << std::endl;
	}

	ui->widget->GetRenderWindow()->Render();
}

void MainWindow::save_output()
{
	//timer used to limit rate of writes/filter updates
	if(!m_timer_timed_out) return;
	m_timer_timed_out = false;

	if(!m_output_set)
		return;
	try {
	    m_writer->Update();
	}catch(itk::ExceptionObject & err)
	{
	    std::cerr << "Exception Caught while writing\n";
	    std::cerr << err << std::endl;
	}

	updateVTKWidget();

	
}

void MainWindow::update_variance()
{
	m_filter->SetVariance(ui->sb_variance->value());
	updateVTKWidget();
}

void MainWindow::toggle_auto_threshold()
{
	if(ui->checkbox_autothreshold->isChecked()){
		ui->sb_upperthresh->setEnabled(false);
		ui->sb_lowerthresh->setEnabled(false);
		m_filter->SetAutomaticThresholdingOn();
	}else{
		ui->sb_upperthresh->setEnabled(true);
		ui->sb_lowerthresh->setEnabled(true);
		m_filter->SetAutomaticThresholdingOff();
	}
	updateVTKWidget();
}

void MainWindow::update_upper_thresh()
{
	m_filter->SetUpperThreshold(ui->sb_upperthresh->value());
	updateVTKWidget();
}

void MainWindow::update_lower_thresh()
{
	m_filter->SetLowerThreshold(ui->sb_lowerthresh->value());
	updateVTKWidget();
}

void MainWindow::timeout();