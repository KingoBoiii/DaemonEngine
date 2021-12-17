pipeline {
    agent any
    stages {
        stage('GenerateProj') {
            steps {
                echo "Generating Projects..."
                bat(script: 'scripts/GenerateProjects.bat')
                echo "Successfully generated projects..."
            }
        }
        stage('Compile Release') {
            steps {
                echo "Compiling release configuration..."
                bat(script: 'scripts/Compile-Release.bat')
                echo "Successfully Compiled release configuration..."
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying zip folder with KingoEditor and assets'
                bat(script: 'scripts/GenerateZip.bat')
                archiveArtifacts artifacts: 'bin\\Release-windows-x86_64\\KingoEditor\\*.zip', fingerprint: true
            }
        }
    }
}