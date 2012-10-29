namespace KinectTableExampleCSharp
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.pictureBoxColor = new System.Windows.Forms.PictureBox();
            this.pictureBoxDepth = new System.Windows.Forms.PictureBox();
            this.pictureBoxDemo = new System.Windows.Forms.PictureBox();
            this.pictureBoxTest = new System.Windows.Forms.PictureBox();
            this.label1 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxColor)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxDepth)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxDemo)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxTest)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBoxColor
            // 
            this.pictureBoxColor.Location = new System.Drawing.Point(12, 12);
            this.pictureBoxColor.Name = "pictureBoxColor";
            this.pictureBoxColor.Size = new System.Drawing.Size(417, 309);
            this.pictureBoxColor.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBoxColor.TabIndex = 0;
            this.pictureBoxColor.TabStop = false;
            // 
            // pictureBoxDepth
            // 
            this.pictureBoxDepth.Location = new System.Drawing.Point(435, 12);
            this.pictureBoxDepth.Name = "pictureBoxDepth";
            this.pictureBoxDepth.Size = new System.Drawing.Size(417, 309);
            this.pictureBoxDepth.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBoxDepth.TabIndex = 0;
            this.pictureBoxDepth.TabStop = false;
            // 
            // pictureBoxDemo
            // 
            this.pictureBoxDemo.Location = new System.Drawing.Point(12, 327);
            this.pictureBoxDemo.Name = "pictureBoxDemo";
            this.pictureBoxDemo.Size = new System.Drawing.Size(417, 309);
            this.pictureBoxDemo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBoxDemo.TabIndex = 0;
            this.pictureBoxDemo.TabStop = false;
            // 
            // pictureBoxTest
            // 
            this.pictureBoxTest.Location = new System.Drawing.Point(435, 327);
            this.pictureBoxTest.Name = "pictureBoxTest";
            this.pictureBoxTest.Size = new System.Drawing.Size(417, 309);
            this.pictureBoxTest.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBoxTest.TabIndex = 0;
            this.pictureBoxTest.TabStop = false;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 642);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(159, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Press key \'t\' to recalculate table.";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(864, 664);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.pictureBoxDepth);
            this.Controls.Add(this.pictureBoxTest);
            this.Controls.Add(this.pictureBoxDemo);
            this.Controls.Add(this.pictureBoxColor);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxColor)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxDepth)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxDemo)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxTest)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBoxColor;
        private System.Windows.Forms.PictureBox pictureBoxDepth;
        private System.Windows.Forms.PictureBox pictureBoxDemo;
        private System.Windows.Forms.PictureBox pictureBoxTest;
        private System.Windows.Forms.Label label1;
    }
}

