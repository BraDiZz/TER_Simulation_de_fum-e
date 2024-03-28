// Configuration du transform feedback
GLuint feedbackBuffer;
glGenBuffers(1, &feedbackBuffer);
glBindBuffer(GL_ARRAY_BUFFER, feedbackBuffer);
glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numParticles * 7, nullptr, GL_DYNAMIC_COPY); // 3 float pour position, 3 float pour vélocité, 1 float pour âge
glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, feedbackBuffer);

// Commencer le transform feedback
glBeginTransformFeedback(GL_POINTS);

// Activer le shader pour la mise à jour des particules
glUseProgram(updateShaderProgram);

// Passer les uniformes nécessaires

// Activer les attributs de sommet nécessaires
// ...

// Commencer le rendu
glDrawArrays(GL_POINTS, 0, numParticles);

// Fin du transform feedback
glEndTransformFeedback();

// Lire les données capturées depuis les tampons de retour
GLfloat* feedbackData = new GLfloat[numParticles * 7];
glBindBuffer(GL_ARRAY_BUFFER, feedbackBuffer);
glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * numParticles * 7, feedbackData);

// Traitement des données capturées
// ...

// Nettoyage
delete[] feedbackData;
glDeleteBuffers(1, &feedbackBuffer);
